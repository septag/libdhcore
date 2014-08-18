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

#ifndef __STACKALLOC_H__
#define __STACKALLOC_H__

#include "types.h"
#include "allocator.h"
#include "core-api.h"
#include "stack.h"

#define STACKALLOC_SAVES_MAX    16

/**
 * Stack allocator: variable-size sequential stack allocator, total size is fixed\n
 * It is the fastest allocator, but it's sequential and does not support dynamic free\n
 * Normal stack allocator, can save and load it's memory offset up to STACKALLOC_SAVES_MAX times,
 * using A_LOAD and A_SAVE macros\n
 * Stack size is fixed, so when user request larger memory than the stack contains, it will throw
 * a warning and allocate the block from heap instead
 * @ingroup alloc
 */
struct stack_alloc
{
    uint8* buffer;
    size_t offset;     /* in bytes */
    size_t last_offset;/* in bytes */
    size_t size;       /* in bytes */
    size_t alloc_max;
    struct allocator* alloc;
    struct stack* save_stack;   /* save stack, data: (size_t) offset to last save */
    uint save_iter;
    struct stack save_nodes[STACKALLOC_SAVES_MAX];
    struct stack* save_ptrs[STACKALLOC_SAVES_MAX];
};

/**
 * Create stack allocator
 * @param alloc allocator for internal stack allocator buffer
 * @param size size of stack allocator buffer (bytes)
 * @ingroup alloc
 */
CORE_API result_t mem_stack_create(struct allocator* alloc,
                                   struct stack_alloc* stack, size_t size, uint mem_id);

/**
 * Destroy stack allocator
 * @ingroup alloc
 */
CORE_API void mem_stack_destroy(struct stack_alloc* stack);

/**
 * Allocate memory from stack allocator
 * @see mem_stack_bindalloc @ingroup alloc
 */
CORE_API void* mem_stack_alloc(struct stack_alloc* stack, size_t size, uint mem_id);

CORE_API void* mem_stack_realloc(struct stack_alloc* stack, void *p, size_t size, uint mem_id);

/**
 * Allocate aligned memory from stack allocator
 * @see mem_stack_bindalloc
 * @ingroup alloc
 */
CORE_API void* mem_stack_alignedalloc(struct stack_alloc* stack, size_t size,
                                      uint8 alignment, uint mem_id);

CORE_API void* mem_stack_alignedrealloc(struct stack_alloc* stack, void *p, size_t size,
                                      uint8 alignment, uint mem_id);

/**
 * save stack allocator state in order to load it later
 * @see mem_stack_load
 * @ingroup alloc
 */
CORE_API void mem_stack_save(struct stack_alloc* stack);

/**
 * Load previously saved stack allocator state.\n
 * memory after saved state is discarded after 'load'
 * @see mem_stack_save
 * @ingroup alloc
 */
CORE_API void mem_stack_load(struct stack_alloc* stack);

/**
 * Reset stack allocator state, discarding any memory that is allocated
 * @ingroup alloc
 */
CORE_API void mem_stack_reset(struct stack_alloc* stack);

/**
 * Free memory from stack, this actually frees only out-of-bound memory block that is allocated
 * from heap instead
 * @ingroup alloc
 */
void mem_stack_free(struct stack_alloc* stack, void* ptr);

/**
 * Free aligned memory from stack, this actually frees only out-of-bound memory block that is allocated
 * from heap instead
 * @ingroup alloc
 */
void mem_stack_alignedfree(struct stack_alloc* stack, void* ptr);

/**
 * bind stack-alloc to generic allocator
 * @ingroup alloc
 */
CORE_API void mem_stack_bindalloc(struct stack_alloc* stack, struct allocator* alloc);

#ifdef __cplusplus

#include "mem-mgr.h"

namespace dh {

class StackAlloc
{
private:
    stack_alloc m_stack;

public:
    StackAlloc()
    {
        memset(&m_stack, 0x00, sizeof(m_stack));
    }

    result_t create(size_t size, allocator *alloc = mem_heap(), uint mem_id = 0)
    {
        return mem_stack_create(alloc, &m_stack, size, mem_id);
    }

    void destroy()
    {
        mem_stack_destroy(&m_stack);
    }

    void push()
    {
        mem_stack_save(&m_stack);
    }

    void pop()
    {
        mem_stack_load(&m_stack);
    }

    void* alloc(size_t size, uint mem_id = 0)
    {
        return mem_stack_alloc(&m_stack, size, mem_id);
    }

    void* realloc(void *p, size_t size, uint mem_id = 0)
    {
        return mem_stack_realloc(&m_stack, p, size, mem_id);
    }

    void* alloc_aligned(size_t size, uint8 align=_ALIGN_DEFAULT_, uint mem_id = 0)
    {
        return mem_stack_alignedalloc(&m_stack, size, align, mem_id);
    }

    void* realloc_aligned(void *p, size_t size, uint8 align=_ALIGN_DEFAULT_, uint mem_id = 0)
    {
        return mem_stack_alignedrealloc(&m_stack, p, size, align, mem_id);
    }

    void free(void *ptr)
    {
        mem_stack_free(&m_stack, ptr);
    }

    void free_aligned(void *ptr)
    {
        mem_stack_alignedfree(&m_stack, ptr);
    }

    void bindto(allocator *alloc)
    {
        mem_stack_bindalloc(&m_stack, alloc);
    }

    void reset()
    {
        mem_stack_reset(&m_stack);
    }
};

}
#endif

#endif /*__STACKALLOC_H__*/
