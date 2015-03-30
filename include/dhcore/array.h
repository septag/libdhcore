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
    int item_cnt;   /**< current item count in the array */
    int max_cnt;    /**< maximum item count */
    int item_sz;    /**< item size in bytes */
    int expand_sz;  /**< in number of items */
    uint mem_id;     /**< memory id */

#ifdef __cplusplus
    array()
    {
        alloc = nullptr;
        buffer = nullptr;
        item_cnt = 0;
        max_cnt = 0;
        item_sz = 0;
        mem_id = 0;
        expand_sz = 0;
    }
#endif
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
CORE_API result_t arr_create(struct allocator* alloc, struct array* arr, int item_sz,
    int init_item_cnt, int expand_cnt, uint mem_id);

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
 * Adds a batch of objects to the array contained.\n
 * Expands if needed and returns a pointer to newly added object
 * @param item_cnt Count of new items to add to array
 * @return newly created pointer to last item in the buffer, @e NULL if out of memory
 */
CORE_API void* arr_add_batch(struct array *arr, int item_cnt);

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

#ifdef __cplusplus
#include "err.h"
#include "mem-mgr.h"
#include "stack.h"

// Must define this for any class/struct that needs to a MutableArray type
#define MUTABLE_ARRAY_ITEM() \
    public: \
    dh::MutableArrayItem m_array_item;  \
    int array_index() const   {   return m_array_item.index;  }


namespace dh {

// Imutable array: objects can be removed from array
// Limitations: Container type must not do anything in constructor/destructor. All operations are
// in memory (memcpy, malloc), so no c++ stuff will happen on add/remove
template <typename T>
class Array
{
private:
    array m_arr;

public:
    Array()
    {
    }

    result_t create(int item_cnt, int expand_cnt, uint mem_id = 0, Allocator *alloc = mem_heap())
    {
        return arr_create(alloc, &m_arr, sizeof(T), item_cnt, expand_cnt, mem_id);
    }

    void destroy()
    {
        arr_destroy(&m_arr);
    }

    T* add()
    {
        return static_cast<T*>(arr_add(&m_arr));
    }

    T* add_batch(int item_cnt)
    {
        return static_cast<T*>(arr_add_batch(&m_arr, item_cnt));
    }

    bool empty() const
    {
        return arr_isempty(&m_arr);
    }

    void clear()
    {
        arr_clear(&m_arr);
    }

    int count() const
    {
        return ARR_COUNT(m_arr);
    }

    int find(const T& item) const
    {
        for (int i = 0; i < m_arr.item_cnt; i++)    {
            if (item == ARR_ITEM(m_arr, T, i))
                return i;
        }
        return -1;
    }

    T* item(int idx)
    {
        ASSERT(idx < ARR_COUNT(m_arr));
        return &ARR_ITEM(m_arr, T, idx);
    }

    T& operator[](int idx)
    {
        ASSERT(idx < ARR_COUNT(m_arr));
        return ARR_ITEM(m_arr, T, idx);
    }

    const T& operator[](int idx) const
    {
        ASSERT(idx < ARR_COUNT(m_arr));
        return ARR_ITEM(m_arr, T, idx);
    }

    operator const array*() const  {    return &m_arr;  }
    operator array*()   {   return &m_arr;  }
    operator const T*() const   {   reinterpret_cast<T*>(m_arr.buffer); }
    operator T*() { return reinterpret_cast<T*>(m_arr.buffer);  }
};

struct MutableArrayItem
{
    int index;
    Stack<int> snode;
};

// Mutable array: objects can be removed from array
// Limitations: Container type must not do anything in constructor/destructor. All operations are
// in memory (memcpy, malloc), so no c++ stuff will happen on add/remove
template <typename _T>
class MutableArray
{
private:
    Array<_T> m_array;
    Stack<int> *m_freeitems = nullptr;
    int m_count = 0;

public:
    MutableArray() = default;

    result_t create(int item_cnt, int expand_cnt, uint mem_id = 0, allocator *alloc = mem_heap())
    {
        return m_array.create(item_cnt, expand_cnt, mem_id, alloc);
    }

    void destroy()
    {
        m_array.destroy();
    }

    _T* add()
    {
        Stack<int> *freeitem = Stack<int>::pop(&m_freeitems);
        m_count ++;
        _T *item;
        if (freeitem)   {
            item = m_array.item(freeitem->data());
        }   else    {
            int index = m_array.count();
            item = m_array.add();
            item->m_array_item.index = index;
        }

        return item;
    }

    void remove(int index)
    {
        // Don't delete it/rearrange from memory, just add the item to freelist
        // This keeps the other indexes intact
        _T *item = m_array.item(index);
        Stack<int>::push(&m_freeitems, &item->m_array_item.snode, index);
        item->m_array_item.index = -1;      // Invalidate

        m_count --;
    }

    bool empty() const
    {
        return m_count == 0;
    }

    void clear()
    {
        m_array.clear();
        m_freeitems = nullptr;
    }

    int count() const
    {
        return m_count;
    }

    int find(const _T& item) const
    {
        int idx = m_array.find(item);
        if (idx != -1)  {
            // Array item must not be invalid
            if (m_array.item(idx)->m_array_item.index == -1)
                idx = -1;
        }
        return idx;
    }

    _T* item(int index)
    {
        ASSERT(m_array[index].m_array_item.index != -1);
        return m_array.item(index);
    }

    _T& operator[](int index)
    {
        return m_array.item(index);
    }

    const _T& operator[](int index) const
    {
        return m_array.item(index);
    }

    operator const _T*() const   {   return (const _T*)m_array; }
};

}   /* dh */
#endif

#endif /*__ARRAY_H__*/
