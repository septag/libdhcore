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

#ifndef __FREELIST_ALLOC_H__
#define __FREELIST_ALLOC_H__

#include "types.h"
#include "allocator.h"
#include "linked-list.h"
#include "core-api.h"

/**
 * freelist allocator: variable-sized small block memory allocator\n
 * more that 8k memory blocks will be allocated from heap
 * @ingroup alloc
 */
struct freelist_alloc
{
    uint8* buffer;
    size_t size;
    size_t alloc_size;
    struct linked_list* free_chunks;
    struct linked_list* alloc_chunks;
    struct allocator*   alloc;

#ifdef __cplusplus
    freelist_alloc()
    {
        buffer = NULL;
        size = 0;
        alloc_size = 0;
        free_chunks = NULL;
        alloc_chunks = NULL;
        alloc = NULL;
    }
#endif
};

/**
 * freelist create/destroy
 * @param alloc allocator for internal freelist memory
 * @param size size (in bytes) for freelist buffer
 * @see mem_freelist_destroy    @ingroup alloc
 */
CORE_API result_t mem_freelist_create(struct allocator* alloc,
                                      struct freelist_alloc* freelist,
                                      size_t size, uint mem_id);
/**
 * destroy freelist
 * @ingroup alloc
 */
CORE_API void mem_freelist_destroy(struct freelist_alloc* freelist);

/**
 * allocate memory from freelist
 * @param size size (in bytes) of requested memory, if requested size is more than 8k -
 * see (freelist-alloc.c), memory will be allocated from heap instead of freelist
 * @return allocated memory block   @ingroup alloc
 */
CORE_API void* mem_freelist_alloc(struct freelist_alloc* freelist, size_t size, uint mem_id);

/**
 * Aligned allocation from freelist
 * @see mem_freelist_alloc
 * @ingroup alloc
 */
CORE_API void* mem_freelist_alignedalloc(struct freelist_alloc* freelist, size_t size,
                                         uint8 alignment, uint mem_id);

/**
 * @ingroup alloc
 */
CORE_API void mem_freelist_free(struct freelist_alloc* freelist, void* ptr);

/**
 * @ingroup alloc
 */
CORE_API void mem_freelist_alignedfree(struct freelist_alloc* freelist, void* ptr);

/**
 * get freelist memory leaks
 * @param pptrs array of pointers to the leaks, if =NULL function only returns number of leaks
 * @return number of leaks
 * @ingroup alloc
 */
CORE_API int mem_freelist_getleaks(struct freelist_alloc* freelist, void** pptrs);

/**
 * get size of the allocated memory from freelist
 */
CORE_API size_t mem_freelist_getsize(struct freelist_alloc* freelist, void* ptr);

/**
 * bind freelist-alloc to generic allocator
 * @ingroup alloc
 */
CORE_API void mem_freelist_bindalloc(struct freelist_alloc* freelist, struct allocator* alloc);

#ifdef __cplusplus

#include "mem-mgr.h"

namespace dh {

class FreelistAlloc
{
private:
    freelist_alloc m_fl;

public:
    FreelistAlloc()
    {
    }

    result_t create(size_t size, uint mem_id = 0, allocator *alloc = mem_heap())
    {
        return mem_freelist_create(alloc, &m_fl, size, mem_id);
    }

    void destroy()
    {
        mem_freelist_destroy(&m_fl);
    }

    void* alloc(size_t size, uint mem_id = 0)
    {
        mem_freelist_alloc(&m_fl, size, mem_id);
    }

    void free(void *ptr)
    {
        mem_freelist_free(&m_fl, ptr);
    }

    void* alloc_aligned(size_t size, uint8 align = 16, uint mem_id = 0)
    {
        mem_freelist_alignedalloc(&m_fl, size, align, mem_id);
    }

    void free_aligned(void *ptr)
    {
        mem_freelist_alignedfree(&m_fl, ptr);
    }

    void bindto(allocator *alloc)
    {
        mem_freelist_bindalloc(&m_fl, alloc);
    }

    int leaks(void **pptrs = NULL)
    {
        return mem_freelist_getleaks(&m_fl, pptrs);
    }
};

} /* dh */
#endif

#endif
