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

#include <stdio.h>
#include "mem-mgr.h"
#include "stack-alloc.h"
#include "err.h"
#include "log.h"

/*************************************************************************************************/
/* functions for binding allocators to stack-alloc */
static void* s_alloc(size_t size, const char* source, uint line, uint mem_id, void* param)
{
    return mem_stack_alloc((struct stack_alloc*)param, size, mem_id);
}

static void* s_realloc(void *p, size_t size, const char* source, uint line, uint mem_id, void* param)
{
    return mem_stack_realloc((struct stack_alloc*)param, p, size, mem_id);
}

static void s_free(void* p, void* param)
{
    mem_stack_free((struct stack_alloc*)param, p);
}

static void* s_alignedalloc(size_t size, uint8 alignment, const char* source, uint line,
                            uint mem_id, void* param)
{
    return mem_stack_alignedalloc((struct stack_alloc*)param, size, alignment, mem_id);
}

static void* s_alignedrealloc(void *p, size_t size, uint8 alignment, const char* source, uint line,
                            uint mem_id, void* param)
{
    return mem_stack_alignedrealloc((struct stack_alloc*)param, p, size, alignment, mem_id);
}

static void s_alignedfree(void* p, void* param)
{
    mem_stack_alignedfree((struct stack_alloc*)param, p);
}

static void s_save(void* param)
{
    mem_stack_save((struct stack_alloc*)param);
}

static void s_load(void* param)
{
    mem_stack_load((struct stack_alloc*)param);
}

/* */
result_t mem_stack_create(struct allocator* alloc, struct stack_alloc* stack,
                          size_t size, uint mem_id)
{
    stack->buffer = (uint8*)A_ALIGNED_ALLOC(alloc, size, mem_id);
    if (stack->buffer == NULL)
        return RET_OUTOFMEMORY;

    stack->size = size;
    stack->alloc = alloc;
    stack->offset = 0;

    for (uint i = 0; i < STACKALLOC_SAVES_MAX; i++)
        stack->save_ptrs[STACKALLOC_SAVES_MAX-i-1] = &stack->save_nodes[i];
    stack->save_iter = STACKALLOC_SAVES_MAX;

    return RET_OK;
}

void mem_stack_destroy(struct stack_alloc* stack)
{
    ASSERT(stack != NULL);

    if (stack->buffer != NULL)  {
        A_ALIGNED_FREE(stack->alloc, stack->buffer);
    }
}

void* mem_stack_alignedalloc(struct stack_alloc* stack, size_t size, uint8 alignment, uint mem_id)
{
    size_t ns = size + alignment;
    uptr_t raw_addr = (uptr_t)mem_stack_alloc(stack, ns, mem_id);
    if (raw_addr == 0)
        return NULL;

    uptr_t misalign = raw_addr & (alignment - 1);
    uint8 adjust = alignment - (uint8)misalign;
    uptr_t aligned_addr = raw_addr + adjust;
    uint8* a = (uint8*)(aligned_addr - sizeof(uint8));
    *a = adjust;
    return (void*)aligned_addr;
}

void* mem_stack_alignedrealloc(struct stack_alloc* stack, void *p, size_t size, uint8 alignment,
                               uint mem_id)
{
    uptr_t aligned_addr;
    uint8 adjust;
    uptr_t raw_addr;
    size_t ns = size + alignment;

    if (p)  {
        aligned_addr = (uptr_t)p;
        adjust = *((uint8*)(aligned_addr - sizeof(uint8)));
        raw_addr = (uptr_t)mem_stack_realloc(stack, (void*)(aligned_addr - adjust), ns, mem_id);
    }   else    {
        raw_addr = (uptr_t)mem_stack_realloc(stack, p, ns, mem_id);
    }

    if (raw_addr == 0)
        return NULL;

    uptr_t misalign = raw_addr & (alignment - 1);
    adjust = alignment - (uint8)misalign;
    aligned_addr = raw_addr + adjust;
    uint8* a = (uint8*)(aligned_addr - sizeof(uint8));
    *a = adjust;
    return (void*)aligned_addr;
}

void* mem_stack_alloc(struct stack_alloc* stack, size_t size, uint mem_id)
{
    ASSERT(stack->buffer != NULL);

    if ((stack->offset + size) > stack->size)   {
#if defined(_DEBUG_)
        printf("Warning: (Performance) stack allocator '%p' (req-size: %d, id: %d) is overloaded."
            "Allocating from heap.\n", stack, (uint)size, mem_id);
#endif
        return ALLOC(size, mem_id);
    }

    void* ptr = stack->buffer + stack->offset;
    stack->offset += size;

    /* save maximum allocated size */
    if (stack->offset > stack->alloc_max)
        stack->alloc_max = stack->offset;

    return ptr;
}

void* mem_stack_realloc(struct stack_alloc* stack, void *p, size_t size, uint mem_id)
{
    ASSERT(stack->buffer != NULL);

    if (p == NULL)
        return mem_stack_alloc(stack, size, mem_id);

    if ((stack->offset + size) > stack->size)   {
#if defined(_DEBUG_)
        printf("Warning: (Performance) stack allocator '%p' (req-size: %d, id: %d) is overloaded."
            "Allocating from heap.\n", stack, (uint)size, mem_id);
#endif
        return ALLOC(size, mem_id);
    }

    uptr_t poffset = (uptr_t)(p - (void*)stack->buffer);
    if (poffset == stack->last_offset)   {
        stack->offset += size;
        return stack->buffer + stack->last_offset;
    }   else    {
        void* ptr = stack->buffer + stack->offset;
        stack->last_offset = stack->offset;
        stack->offset += size;
        /* save maximum allocated size */
        if (stack->offset > stack->alloc_max)
            stack->alloc_max = stack->offset;
        return ptr;
    }
}

void mem_stack_alignedfree(struct stack_alloc* stack, void* ptr)
{
    uptr_t aligned_addr = (uptr_t)ptr;
    uint8 adjust = *((uint8*)(aligned_addr - sizeof(uint8)));
    uptr_t raw_addr = aligned_addr - adjust;
    mem_stack_free(stack, (void*)raw_addr);
}

void mem_stack_free(struct stack_alloc* stack, void* ptr)
{
    uptr_t nptr = (uptr_t)ptr;
    uptr_t nbuff = (uptr_t)stack->buffer;
    if (nptr < nbuff || nptr >= (nbuff + stack->size))
        FREE(ptr);
}

void mem_stack_bindalloc(struct stack_alloc* stack, struct allocator* alloc)
{
    alloc->param = stack;
    alloc->alloc_fn = s_alloc;
    alloc->realloc_fn = s_realloc;
    alloc->alignedalloc_fn = s_alignedalloc;
    alloc->alignedrealloc_fn = s_alignedrealloc;
    alloc->free_fn = s_free;
    alloc->alignedfree_fn = s_alignedfree;
    alloc->save_fn = s_save;
    alloc->load_fn = s_load;
}

void mem_stack_save(struct stack_alloc* stack)
{
    if (stack->save_iter == 0)  {
        ASSERT(0);  /* Too much saves without load,
                     * increase STACKALLOC_SAVES_MAX
                     * or check your code for A_SAVE without proceeding A_LOAD */
        return;
    }

    struct stack* snode = stack->save_ptrs[--stack->save_iter];
    stack_push(&stack->save_stack, snode, (void*)stack->offset);
    stack->last_offset = stack->offset;
}

void mem_stack_load(struct stack_alloc* stack)
{
    struct stack* snode = stack_pop(&stack->save_stack);
    size_t save_offset = (size_t)snode->data;
    ASSERT(save_offset <= stack->offset);

    stack->offset = save_offset;
    stack->last_offset = save_offset;
    stack->save_ptrs[stack->save_iter++] = snode;
    ASSERT(stack->save_iter <= STACKALLOC_SAVES_MAX);   /* In case of error,
                                                         * It's likely that you didn't call each
                                                         * A_LOAD with proceeding A_SAVE */
}

void mem_stack_reset(struct stack_alloc* stack)
{
    for (int i = 0; i < STACKALLOC_SAVES_MAX; i++)
        stack->save_ptrs[STACKALLOC_SAVES_MAX-i-1] = &stack->save_nodes[i];
    stack->save_iter = STACKALLOC_SAVES_MAX;
    stack->offset = 0;
    stack->last_offset = 0;
    stack->save_stack = NULL;
}

