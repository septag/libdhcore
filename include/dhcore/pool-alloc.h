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
#include "mt.h"

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
    uint blocks_cnt; /* count of memory pool blocks */
    struct allocator* alloc;      /* allocator for further block allocations */
    uint mem_id;     /* memory id of the pool */
    uint items_max;  /* maximum number of items allowed (per block) */
    uint item_sz;    /* size of the item in bytes */
};

/**
 * Creates a fixed item size pool and it's buffer
 * @param item_size size of each item (bytes) in the pool
 * @param block_size number of items in each pool block
 * @ingroup alloc
 */
CORE_API result_t mem_pool_create(struct allocator* alloc,
                                  struct pool_alloc* pool,
                                  uint item_size, uint block_size, uint mem_id);

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
CORE_API uint mem_pool_getleaks(struct pool_alloc* pool);

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

/**
 * Pool allocator: fixed-size pool allocation (thread-safe)\n
 * it is pretty fast and can dynamically grow itself on demand. but limited to fixed sized blocks\n
 * if number of allocations go beyond 'block_size' another block will be created
 * @see mem_pool_create
 * @ingroup alloc
 */
struct pool_alloc_ts
{
    struct pool_alloc p;
    mt_mutex lock;
};

/**
 * Creates a fixed item size pool and it's buffer (thread-safe)
 * @param item_size size of each item (bytes) in the pool
 * @param block_size number of items in each pool block
 * @ingroup alloc
 */
CORE_API result_t mem_pool_create_ts(struct allocator* alloc, struct pool_alloc_ts* pool,
                                     uint item_size, uint block_size, uint mem_id);

/**
 * Destroys pool allocator (thread-safe)
 * @ingroup alloc
 */
CORE_API void mem_pool_destroy_ts(struct pool_alloc_ts* pool);

/**
 * Allocate an item (fixed-size) from the pool (thread-safe)
 * @ingroup alloc
 */
CORE_API void* mem_pool_alloc_ts(struct pool_alloc_ts* pool);

/**
 * Free an item from the pool (thread-safe)
 * @ingroup alloc
 */
CORE_API void mem_pool_free_ts(struct pool_alloc_ts* pool, void* ptr);

/**
 * Get memory pool leaks (thread-safe)
 * @return number of leaks
 * @ingroup alloc
 */
CORE_API uint mem_pool_getleaks_ts(struct pool_alloc_ts* pool);

/**
 * Clear memory pool (thread-safe)
 * @ingroup alloc
 */
CORE_API void mem_pool_clear_ts(struct pool_alloc_ts* pool);

/**
 * Pool binding to generic allocator (thread-safe)
 * @ingroup alloc
 */
CORE_API void mem_pool_bindalloc_ts(struct pool_alloc_ts* pool, struct allocator* alloc);

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
        memset(&m_pool, 0x00, sizeof(m_pool));
    }

    result_t create(uint block_sz, allocator *alloc = mem_heap(), uint mem_id = 0)
    {
        mem_pool_create(alloc, &m_pool, sizeof(T), block_sz, mem_id);
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

    uint leaks()
    {
        return mem_pool_getleaks(&m_pool);
    }
};

} /* dh */
#endif

#endif /* __POOLALLOC_H__ */
