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

#include "dhcore/types.h"
#include "dhcore/pool-alloc.h"
#include "dhcore/err.h"
#include "dhcore/mem-mgr.h"

struct ALIGN16 mem_pool_block
{
    struct linked_list node; /* linked-list node */
    uint8* buffer; /* memory buffer that holds all objects */
    void** ptrs; /* pointer references to the buffer */
    int iter; /* iterator for current buffer position */
};


/* fwd declarations */
static struct mem_pool_block* pool_create_singleblock(struct pool_alloc* pool, int item_size,
                                                      int block_size);
static void pool_destroy_singleblock(struct pool_alloc* pool, struct mem_pool_block* block);

/* callback functions for binding pool-alloc to generic allocator */
static void* p_alloc(size_t size, const char* source, uint line, uint mem_id, void* param)
{
    ASSERT(((struct pool_alloc*)param)->item_sz == (int)size);
    return mem_pool_alloc((struct pool_alloc*)param);
}

static void* p_realloc(void *p, size_t size, const char* source, uint line, uint mem_id, void* param)
{
    ASSERT(((struct pool_alloc*)param)->item_sz == (int)size);
    if (p)
        mem_pool_free((struct pool_alloc*)param, p);
    return mem_pool_alloc((struct pool_alloc*)param);
}

static void p_free(void* p, void* param)
{
    mem_pool_free((struct pool_alloc*)param, p);
}

static void* p_alignedalloc(size_t size, uint8 alignment, const char* source, uint line,
                            uint mem_id, void* param)
{
    ASSERT(((struct pool_alloc*)param)->item_sz == (int)size);
    return mem_pool_alloc((struct pool_alloc*)param);
}

static void* p_alignedrealloc(void *p, size_t size, uint8 alignment, const char* source, uint line,
                              uint mem_id, void* param)
{
    ASSERT(((struct pool_alloc*)param)->item_sz == (int)size);

    if (p)
        mem_pool_free((struct pool_alloc*)param, p);
    return mem_pool_alloc((struct pool_alloc*)param);
}

static void p_alignedfree(void* p, void* param)
{
    mem_pool_free((struct pool_alloc*)param, p);
}

/* */
result_t mem_pool_create(struct allocator* alloc, struct pool_alloc* pool, int item_size,
                         int block_size, uint mem_id)
{
    struct mem_pool_block *block;

    memset(pool, 0x00, sizeof(struct pool_alloc));
    pool->item_sz = item_size;
    pool->items_max = block_size;
    pool->mem_id = mem_id;
    pool->alloc = alloc;

    /* create the first block */
    block = pool_create_singleblock(pool, item_size, block_size);
    if (block == NULL)  {
        mem_pool_destroy(pool);
        return RET_OUTOFMEMORY;
    }

    return RET_OK;
}

void mem_pool_destroy(struct pool_alloc* pool)
{
    /* destroy all blocks of memory pool */
    struct linked_list* node = pool->blocks;
    while (node != NULL)    {
        struct linked_list* next = node->next;
        pool_destroy_singleblock(pool, (struct mem_pool_block*)node->data);
        node = next;
    }
}

static struct mem_pool_block* pool_create_singleblock(struct pool_alloc* pool, int item_size,
                                                      int block_size)
{
    size_t total_sz =
        sizeof(struct mem_pool_block) +
        item_size*block_size +
        sizeof(void*)*block_size;
    uint8* buff = (uint8*)A_ALIGNED_ALLOC(pool->alloc, total_sz, pool->mem_id);
    if (buff == NULL)
        return NULL;
    memset(buff, 0x00, total_sz);

    struct mem_pool_block* block = (struct mem_pool_block*)buff;
    buff += sizeof(struct mem_pool_block);
    block->buffer = buff;
    buff += item_size*block_size;
    block->ptrs = (void**)buff;

    /* assign pointer references to buffer */
    for (int i = 0; i < block_size; i++)
        block->ptrs[block_size-i-1] = block->buffer + i*item_size;
    block->iter = block_size;

    /* add to linked-list of the pool */
    list_addlast(&pool->blocks, &block->node, block);
    pool->blocks_cnt++;
    return block;
}

static void pool_destroy_singleblock(struct pool_alloc* pool, struct mem_pool_block* block)
{
    list_remove(&pool->blocks, &block->node);
    A_ALIGNED_FREE(pool->alloc, block);
    pool->blocks_cnt--;
}

void* mem_pool_alloc(struct pool_alloc* pool)
{
    struct mem_pool_block* block;
    struct linked_list* node = pool->blocks;

    while (node != NULL)   {
        block = (struct mem_pool_block*)node->data;
        if (block->iter > 0)
            return block->ptrs[--block->iter];

        node = node->next;
    }

    /* couldn't find a free block, create a new one */
    block = pool_create_singleblock(pool, pool->item_sz, pool->items_max);
    if (block == NULL)
        return NULL;

    return block->ptrs[--block->iter];
}


void mem_pool_free(struct pool_alloc* pool, void* ptr)
{
    /* find the block that pointer belongs to */
    struct linked_list* node = pool->blocks;
    struct mem_pool_block* block;
    int buffer_sz = pool->items_max * pool->item_sz;
    uint8* u8ptr = (uint8*)ptr;

    while (node != NULL)   {
        block = (struct mem_pool_block*)node->data;
        if (u8ptr >= block->buffer && u8ptr < (block->buffer + buffer_sz))  {
            ASSERT(block->iter != pool->items_max);
            block->ptrs[block->iter++] = ptr;
            return;
        }
        node = node->next;
    }

    /* memory block does not belong to the pool?! */
    ASSERT(0);
}

void mem_pool_clear(struct pool_alloc* pool)
{
    int item_size = pool->item_sz;
    int block_size = pool->items_max;

    struct linked_list* node = pool->blocks;
    while (node != NULL)    {
        struct mem_pool_block* block = (struct mem_pool_block*)node->data;

        /* only re-assign pointer references to buffer */
        for (int i = 0; i < block_size; i++)
            block->ptrs[block_size-i-1] = block->buffer + i*item_size;
        block->iter = block_size;

        node = node->next;
    }
}

void mem_pool_bindalloc(struct pool_alloc* pool, struct allocator* alloc)
{
    alloc->param = pool;
    alloc->alloc_fn = p_alloc;
    alloc->realloc_fn = p_realloc;
    alloc->alignedalloc_fn = p_alignedalloc;
    alloc->alignedrealloc_fn = p_alignedrealloc;
    alloc->alignedfree_fn = p_alignedfree;
    alloc->free_fn = p_free;
    alloc->save_fn = NULL;
    alloc->load_fn = NULL;
}

int mem_pool_getleaks(struct pool_alloc* pool)
{
    int count = 0;
    struct linked_list* node = pool->blocks;
    struct mem_pool_block* block;

    while (node != NULL)    {
        block = (struct mem_pool_block*)node->data;
        count += (pool->items_max - block->iter);
        node = node->next;
    }
    return count;
}

