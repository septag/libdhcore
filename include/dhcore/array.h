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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "types.h"
#include "core-api.h"
#include "allocator.h"

/**
 * @defgroup array Array
 * Expanding array\n
 * holds a buffer and it's properties for expansion on demand\n
 * Usage example:\n
 * @code
 * struct array myarr;
 * arr_create(mem_heap(), &myarr, sizeof(uint), 100, 200, 0);
 * printf("adding numbers\n");
 * for (uint i = 0; i < 10; i++)  {
 *      uint* pval = arr_add(&myarr);
 *      *paval = i;
 * }
 * printf("listing numbers ...\n");
 * for (uint i = 0; i < ARR_COUNT(myarr); i++)  {
 *      printf("%d\n", ARR_ITEM(myarr, uint, i));
 * }
 * arr_destroy(&myarr);
 * @endcode
 * @ingroup array
 */

/**
 * Macro that can be used to fetch items in the array
 * @param arr Name of the array object
 * @param type Type of the array item data (for example for Int array, it's @e int)
 * @param idx Index of the array item
 * @return Array item value (It's data type is defined by @type parameter)
 * @ingroup array
 */ 
#define ARR_ITEM(arr, type, idx)    ((type *)(arr).buffer)[(idx)]

/**
 * Returns array's item count 
 * @ingroup array
 */
#define ARR_COUNT(arr)              ((arr).item_cnt)

struct array
{
    struct allocator* alloc;      /**< allocator */
    void* buffer;     /**< array buffer, can be casted to any pointer type for use */
    uint item_cnt;   /**< current item count in the array */
    uint max_cnt;    /**< maximum item count */
    uint item_sz;    /**< item size in bytes */
    uint mem_id;     /**< memory id */
    uint expand_sz;  /**< in number of items */
};

/**
 * Creates an array
 * @param alloc Internal memory allocator for array data (@e mem_heap for default heap allocator)
 * @param arr Array to be created
 * @param item_sz Each array element item size (in bytes)
 * @param init_item_cnt Initial maximum item count
 * @param expand_cnt Number of items to expand if needed
 * @see arr_add
 * @ingroup array
 */
CORE_API result_t arr_create(struct allocator* alloc, struct array* arr, uint item_sz, 
    uint init_item_cnt, uint expand_cnt, uint mem_id);

/**
 * Destroys array
 * @see arr_create
 * @ingroup array
 */
CORE_API void arr_destroy(struct array* arr);

/**
 * Expand the array buffer by @expand_cnt defined in arr_create\n
 * Recommended method is to check to array expansion before adding new items\n
 * Example: 
 * @code 
 * if (arr_needexpand(a))  arr_expand(a)\n
 *     a.buffer[a.item_cnt++] = item
 * @endcode
 * @see arr_needexpand
 * @see arr_create
 * @ingroup array
 */
CORE_API result_t arr_expand(struct array* arr);

/**
 * Adds an object to the array contained.\n
 * Expands if needed and returns a pointer to newly added object
 * @return newly created pointer to last item in the buffer, @e NULL if out of memory
 */
CORE_API void* arr_add(struct array* arr);

/**
 * Checks if array needs expansion (actual item count is equal to maximum item count)
 * @see arr_expand
 * @ingroup array
 */
INLINE int arr_needexpand(const struct array* arr)
{
    return (arr->max_cnt == arr->item_cnt);
}

/**
 * Checks if array is empty
 * @ingroup array
 */
INLINE int arr_isempty(const struct array* arr)
{
    return (arr->item_cnt == 0);
}

/**
 * Clear array items. This function does not shrink the internal buffer.
 * @ingroup array
 */
INLINE void arr_clear(struct array* arr)
{
    arr->item_cnt = 0;
}

#endif /*__ARRAY_H__*/
