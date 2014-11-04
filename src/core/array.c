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

#include "dhcore/array.h"
#include "dhcore/err.h"
#include "dhcore/numeric.h"

result_t arr_create(struct allocator* alloc,
                    struct array* arr,
                    int item_sz, int init_item_cnt, int expand_cnt,
                    uint mem_id)
{
    memset(arr, 0x00, sizeof(struct array));
    arr->buffer = A_ALIGNED_ALLOC(alloc, item_sz*init_item_cnt, mem_id);
    if (arr->buffer == NULL)
        return RET_OUTOFMEMORY;

    arr->alloc = alloc;
    arr->expand_sz = expand_cnt;
    arr->item_cnt = 0;
    arr->item_sz = item_sz;
    arr->max_cnt = init_item_cnt;
    arr->mem_id = mem_id;

    return RET_OK;
}

void arr_destroy(struct array* arr)
{
    ASSERT(arr != NULL);

    if (arr->buffer != NULL)    {
        ASSERT(arr->alloc != NULL);
        A_ALIGNED_FREE(arr->alloc, arr->buffer);
    }
}

void* arr_add(struct array* arr)
{
    result_t r = RET_OK;
    if (arr_needexpand(arr))
        r = arr_expand(arr);

    if (r == RET_OK)    {
        void* p = (uint8*)arr->buffer + arr->item_cnt*arr->item_sz;
        arr->item_cnt ++;
        return p;
    }   else    {
        return NULL;
    }
}

void* arr_add_batch(struct array *arr, int item_cnt)
{
    if (arr->max_cnt < item_cnt + arr->item_cnt)    {
        int newsz = aligni(item_cnt + arr->item_cnt, arr->expand_sz);

        arr->buffer = A_ALIGNED_REALLOC(arr->alloc, arr->buffer, newsz*arr->item_sz, arr->mem_id);
        if (arr->buffer == NULL)
            return NULL;
        arr->max_cnt = newsz;
    }

    void *p = (uint8*)arr->buffer + arr->item_cnt*arr->item_sz;
    arr->item_cnt += item_cnt;

    return p;
}

result_t arr_expand(struct array* arr)
{
    ASSERT(arr != NULL);
    ASSERT(arr->alloc != NULL);
    ASSERT(arr->buffer != NULL);

    /* reallocate */
    int newsz = arr->max_cnt + arr->expand_sz;
    arr->buffer = A_ALIGNED_REALLOC(arr->alloc, arr->buffer, newsz*arr->item_sz, arr->mem_id);
    if (arr->buffer == NULL)
        return RET_OUTOFMEMORY;

    arr->max_cnt = newsz;
    return RET_OK;
}


