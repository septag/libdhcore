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


#ifndef __POOLALLOC_H__
#define __POOLALLOC_H__

#include "types.h"
#include "linked-list.h"
#include "allocator.h"
#include "core-api.h"

/**
 * Pool allocator: fixed-size pool allocation\n
 * it is pretty fast and can dynamically grow itself on demand. but limited to fixed sized blocks\n
 * if number of allocations go beyond 'block_size' another block will be created
 * @see mem_pool_create
 * @ingroup alloc
 */
struct pool_alloc
{
    struct linked_list* blocks;     /* first node of blocks */
    int blocks_cnt; /* count of memory pool blocks */
    struct allocator* alloc;      /* allocator for further block allocations */
    int items_max;  /* maximum number of items allowed (per block) */
    int item_sz;    /* size of the item in bytes */
    uint mem_id;     /* memory id of the pool */

#ifdef __cplusplus
    pool_alloc()
    {
        blocks = NULL;
        blocks_cnt = 0;
        alloc = NULL;
        mem_id = 0;
        items_max = 0;
        item_sz = 0;
    }
#endif
};

/**
 * Creates a fixed item size pool and it's buffer
 * @param item_size size of each item (bytes) in the pool
 * @param block_size number of items in each pool block
 * @ingroup alloc
 */
CORE_API result_t mem_pool_create(struct allocator* alloc,
                                  struct pool_alloc* pool,
                                  int item_size, int block_size, uint mem_id);

/**
 * Destroys pool allocator
 * @ingroup alloc
 */
CORE_API void mem_pool_destroy(struct pool_alloc* pool);

/**
 * Allocate an item (fixed-size) from the pool
 * @ingroup alloc
 */
CORE_API void* mem_pool_alloc(struct pool_alloc* pool);

/**
 * Free an item from the pool
 * @ingroup alloc
 */
CORE_API void mem_pool_free(struct pool_alloc* pool, void* ptr);

/**
 * Get memory pool leaks
 * @return number of leaks
 * @ingroup alloc
 */
CORE_API int mem_pool_getleaks(struct pool_alloc* pool);

/**
 * Clear memory pool
 * @ingroup alloc
 */
CORE_API void mem_pool_clear(struct pool_alloc* pool);

/**
 * Pool binding to generic allocator
 * @ingroup alloc
 */
CORE_API void mem_pool_bindalloc(struct pool_alloc* pool, struct allocator* alloc);


#ifdef __cplusplus
#include "mem-mgr.h"

namespace dh {

template <typename T>
class PoolAlloc
{
private:
    pool_alloc m_pool;

public:
    PoolAlloc()
    {
    }

    result_t create(int block_sz, allocator *alloc = mem_heap(), uint mem_id = 0)
    {
        return mem_pool_create(alloc, &m_pool, sizeof(T), block_sz, mem_id);
    }

    void destroy()
    {
        mem_pool_destroy(&m_pool);
    }

    T* alloc()
    {
        return static_cast<T*>(mem_pool_alloc(&m_pool));
    }

    void free(T *ptr)
    {
        mem_pool_free(&m_pool, ptr);
    }

    void clear()
    {
        mem_pool_clear(&m_pool);
    }

    void bindto(allocator *alloc)
    {
        mem_pool_bindalloc(&m_pool, alloc);
    }

    int leaks()
    {
        return mem_pool_getleaks(&m_pool);
    }
};

} /* dh */
#endif

#endif /* __POOLALLOC_H__ */
