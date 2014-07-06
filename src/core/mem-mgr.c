/***********************************************************************************
 * Copyright (c) 2012, Sepehr Taghdisian
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 ***********************************************************************************/

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#ifdef HAVE_MALLOC_H
  #include <malloc.h>
#endif
#include "mem-mgr.h"
#include "allocator.h"
#include "linked-list.h"
#include "log.h"
#include "str.h"
#include "err.h"
#include "mt.h"

/* global heap allocator */
static struct allocator g_memheap;

/* data that is reserved before each memory block
 * we also have a linked-list to keep track of allocated blocks
 */
struct mem_trace_data
{
    size_t size;

#if defined(_DEBUG_)
    char filename[24];
    uint line;
#endif

    uint mem_id;
    struct linked_list node;
};

struct memid_desc
{
    size_t sum;
    uint id;
};

/* global memory data used in alloc/free functions */
struct mem_mgr
{
    int trace;  /* trace memory ? */
    struct mem_stats stats;  /* memory stats */
    struct linked_list* blocks; /* blocks of memory (linked-list) */
    uint id_cnt;
    uint id_cnt_max;
    struct memid_desc* ids; /* to keep track of each memory ID sum */
    mt_mutex lock;
};

/* globals */
static struct mem_mgr* g_mem = NULL;

/*************************************************************************************************/
/* inline function to allocate data with memory trace block */
static void* mem_alloc_withtrace(size_t size);
/* inline function to free pointer with it's allocated memory trace block*/
static void mem_free_withtrace(void* ptr);
static void mem_addto_ids(uint id, size_t size);
static void mem_removefrom_ids(void* ptr);

/*************************************************************************************************/
INLINE struct mem_trace_data* get_trace_data(void* ptr)
{
    uint8* u8ptr = (uint8*)ptr;
    return (struct mem_trace_data*)(u8ptr - sizeof(struct mem_trace_data));
}

INLINE void* malloc_withsize(size_t s)
{
    void* ptr = malloc(s + sizeof(size_t));
    if (ptr != NULL)    {
        *((size_t*)ptr) = s;
        return ((uint8*)ptr + sizeof(size_t));
    }
    return NULL;
}

INLINE void free_withsize(void* ptr)
{
    void* real_ptr = ((uint8*)ptr - sizeof(size_t));
    free(real_ptr);
}

/*************************************************************************************************/
/* built-in heap allocator callbacks */
static void* heap_alloc(size_t size, const char* source, uint line, uint id, void* param)
{
    return mem_alloc(size, source, line, id);
}

static void heap_free(void* ptr, void* param)
{
    mem_free(ptr);
}

static void* heap_alignedalloc(size_t size, uint8 align, const char* source,
                        uint line, uint id, void* param)
{
    return mem_alignedalloc(size, align, source, line, id);
}

static void heap_alignedfree(void* ptr, void* param)
{
    mem_alignedfree(ptr);
}

/*************************************************************************************************/
/* */
result_t mem_init(int trace_mem)
{
    if (g_mem != NULL)
        return RET_FAIL;
    g_mem = (struct mem_mgr*)malloc(sizeof(struct mem_mgr));
    if (g_mem == NULL)
        return RET_OUTOFMEMORY;
    memset(g_mem, 0x00, sizeof(struct mem_mgr));

    /* initialize default allocators */
    g_memheap.alloc_fn = heap_alloc;
    g_memheap.free_fn = heap_free;
    g_memheap.alignedalloc_fn = heap_alignedalloc;
    g_memheap.alignedfree_fn = heap_alignedfree;
    g_memheap.save_fn = NULL;
    g_memheap.load_fn = NULL;

    g_mem->trace = trace_mem;
    mt_mutex_init(&g_mem->lock);

    g_mem->ids = (struct memid_desc*)malloc(sizeof(struct memid_desc)*16);
    if (g_mem->ids == NULL)
        return RET_OUTOFMEMORY;
    g_mem->id_cnt_max = 16;

    return RET_OK;
}

void mem_release()
{
    if (g_mem != NULL)  {
        if (g_mem->ids == NULL)
            free(g_mem->ids);

    	mt_mutex_release(&g_mem->lock);

        free(g_mem);
        g_mem = NULL;
    }
}

int mem_isinit()
{
    return g_mem != NULL;
}

void* mem_alignedalloc(size_t size, uint8 alignment,
                       const char* source, uint line, uint id)
{
    size_t ns = size + alignment;
    uptr_t raw_addr = (uptr_t)mem_alloc(ns, source, line, id);
    if (raw_addr == 0)     return NULL;

    uptr_t misalign = raw_addr & (alignment - 1);
    uint8 adjust = alignment - (uint8)misalign;
    uptr_t aligned_addr = raw_addr + adjust;
    uint8* a = (uint8*)(aligned_addr - sizeof(uint8));
    *a = adjust;

    return (void*)aligned_addr;
}

void* mem_alloc(size_t size, const char* source, uint line, uint id)
{
    ASSERT(g_mem);

	void* ptr;
	if (g_mem->trace)     {
		mt_mutex_lock(&g_mem->lock);
		if (g_mem->stats.limit_bytes != 0 &&
			(size + g_mem->stats.alloc_bytes) > g_mem->stats.limit_bytes)
		{
			return NULL;
		}

		ptr = mem_alloc_withtrace(size);
		if (ptr != NULL)    {
			struct mem_trace_data* trace = get_trace_data(ptr);
#if defined(_DEBUG_)
			path_getfullfilename(trace->filename, source);
			trace->line = line;
#endif
			trace->size = size;
			trace->mem_id = id;

            mem_addto_ids(id, size);
		}
		mt_mutex_unlock(&g_mem->lock);
    }	else	{
    	ptr = malloc_withsize(size);
    }
    return ptr;
}


void mem_alignedfree(void* ptr)
{
    uptr_t aligned_addr = (uptr_t)ptr;
    uint8 adjust = *((uint8*)(aligned_addr - sizeof(uint8)));
    uptr_t raw_addr = aligned_addr - adjust;
    mem_free((void*)raw_addr);
}

void mem_free(void* ptr)
{
    ASSERT(g_mem);
	if (g_mem->trace)	{
		mt_mutex_lock(&g_mem->lock);
        mem_removefrom_ids(ptr);
		mem_free_withtrace(ptr);
		mt_mutex_unlock(&g_mem->lock);
	}	else	{
		free_withsize(ptr);
	}
}


int mem_isoverrun()
{
    return g_mem->stats.alloc_bytes > g_mem->stats.limit_bytes;
}

void mem_getstats(struct mem_stats* stats)
{
    memcpy(stats, &g_mem->stats, sizeof(struct mem_stats));
}

void mem_reportleaks()
{
    if (g_mem == NULL || !g_mem->trace)
        return;

    size_t leaks_bytes = 0;
    uint leaks_cnt = 0;

    struct linked_list* node = g_mem->blocks;
    if (node != NULL)   
        puts("Memory leaks: ");

    while (node != NULL)    {
        struct mem_trace_data* trace = (struct mem_trace_data*)node->data;
#if defined(_DEBUG_)
        printf("\t%s(line: %d)- (0x%p) %d bytes\n", trace->filename, trace->line, 
            trace + sizeof(struct mem_trace_data), (int)trace->size);
#else
        printf("\t(0x%p) %d bytes (id=%d)\n", trace + sizeof(struct mem_trace_data), (int)trace->size, 
            trace->mem_id);
#endif
        leaks_bytes += trace->size;
        leaks_cnt ++;
        node = node->next;
    }

    if (leaks_cnt > 0)  {
        printf("Found %d leak(s), total %d bytes (%d kb)\n", leaks_cnt, (int)leaks_bytes, 
            (int)leaks_bytes/1024);
    }
}

void mem_setmaxlimit(size_t size)
{
    g_mem->stats.limit_bytes = size;
}

size_t mem_sizebyid(uint id)
{
    if (g_mem && g_mem->trace)    {
        for (uint i = 0, cnt = g_mem->id_cnt; i < cnt; i++)    {
            if (g_mem->ids[i].id == id)
                return g_mem->ids[i].sum;
        }
    }
    return 0;
}

size_t mem_alignedsize(void* ptr)
{
    uptr_t aligned_addr = (uptr_t)ptr;
    uint8 adjust = *((uint8*)(aligned_addr - sizeof(uint8)));
    uptr_t raw_addr = aligned_addr - adjust;
    return mem_size((void*)raw_addr);
}

size_t mem_size(void* ptr)
{
    if (g_mem) {
        if (g_mem->trace)
            return get_trace_data(ptr)->size;
        else
            return *((size_t*)((uint8*)ptr - sizeof(size_t)));
    }   else    {
        return 0;
    }
}

struct allocator* mem_heap()
{
    return &g_memheap;
}

static void* mem_alloc_withtrace(size_t size)
{
    size_t s = size + sizeof(struct mem_trace_data);
    uint8* ptr = (uint8*)malloc(s);
    if (ptr != NULL)    {
        struct mem_trace_data* trace = (struct mem_trace_data*)ptr;

        g_mem->stats.tracer_alloc_bytes += sizeof(struct mem_trace_data);
        g_mem->stats.alloc_cnt++;
        g_mem->stats.alloc_bytes += size;

        list_add(&g_mem->blocks, &trace->node, trace);

        return ptr + sizeof(struct mem_trace_data);
    }

    return NULL;
}

static void mem_free_withtrace(void* ptr)
{
    struct mem_trace_data* trace = get_trace_data(ptr);
    g_mem->stats.alloc_bytes -= trace->size;
    g_mem->stats.alloc_cnt--;
    g_mem->stats.tracer_alloc_bytes -= sizeof(struct mem_trace_data);

    list_remove(&g_mem->blocks, &trace->node);
    free(trace);
}

static void mem_addto_ids(uint id, size_t size)
{
    for (uint i = 0, cnt = g_mem->id_cnt; i < cnt; i++)    {
        if (g_mem->ids[i].id == id)  {
            g_mem->ids[i].sum += size;
            return;
        }
    }

    /* not found in current IDs, add more */
    if (g_mem->id_cnt == g_mem->id_cnt_max)   {
        g_mem->id_cnt_max += 16;
        g_mem->ids = (struct memid_desc*)realloc(g_mem->ids,
            sizeof(struct memid_desc)*g_mem->id_cnt_max);
        ASSERT(g_mem->ids);
    }
    struct memid_desc* desc = &g_mem->ids[g_mem->id_cnt++];
    desc->id = id;
    desc->sum = size;
}

static void mem_removefrom_ids(void* ptr)
{
    struct mem_trace_data* t = get_trace_data(ptr);

    for (uint i = 0, cnt = g_mem->id_cnt; i < cnt; i++)    {
        if (g_mem->ids[i].id == t->mem_id)  {
            g_mem->ids[i].sum -= t->size;
            return;
        }
    }
    ASSERT(0);
}

void mem_heap_bindalloc(struct allocator* alloc)
{
    alloc->param = NULL;
    alloc->alloc_fn = heap_alloc;
    alloc->free_fn = heap_free;
    alloc->alignedalloc_fn = heap_alignedalloc;
    alloc->alignedfree_fn = heap_alignedfree;
}
