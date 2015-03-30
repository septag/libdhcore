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

 
#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"

/**
 * @defgroup stack Stack
 * FILO (first-in-last-out) stack structure\n
 * Usage :\n
 * @code
 * // stack must be wrapped inside your own structures/objects
 * struct myobj
 * {
 *     int data;
 *     struct stack node;
 * };
 * 
 * // we hold the master stack (pointer) to keep track of the stack
 * struct stack* root = NULL;
 * 
 * struct myobj sobj1;
 * struct myobj sobj2;
 * memset(&sobj1, 0x00, sizeof(sobj1));
 * memset(&sobj2, 0x00, sizeof(sobj2));
 * 
 * sobj1.data = 1;
 * sobj2.data = 2;
 * 
 * stack_push(&root, &sobj1.node, &sobj1);
 * stack_push(&root, &sobj2.node, &sobj2);
 * 
 * // ...
 * // pop from stack
 * struct stack* sitem;
 * while ((sitem = stack_pop(&root)) != NULL)  {
 *    struct myobj* obj = sitem->data;
 *    printf("data: %d\n", obj->data);
 * }
 * @endcode
 * @see stack_push
 * @see stack_pop
 * @ingroup stack
 */

struct stack
{
    struct stack*   prev;
    void*           data;

#ifdef __cplusplus
    stack() : prev(NULL), data(NULL)    {}
#endif
};


/**
 * Push item into stack
 * @param pstack pointer to root stack item (can be NULL)
 * @param item stack item to be pushed
 * @param data custom data to keep in stack item, mostly owner of the current stack item
 * @ingroup stack
 */
INLINE void stack_push(struct stack** pstack, struct stack* item, void* data)
{
    item->prev = *pstack;
    *pstack = item;
    item->data = data;
}

/**
 * Pop item from stack
 * @param pstack pointer to root stack item, will be NULL if last item is popped
 * @return popped stack item, look in ->data member variable for owner data
 * @ingroup stack
 */
INLINE struct stack* stack_pop(struct stack** pstack)
{
    struct stack* item = *pstack;
    if (*pstack != NULL)    {
        *pstack = (*pstack)->prev;
        item->prev = NULL;
    }
    return item;
}

#ifdef __cplusplus
namespace dh    {

// _T: container type for stack's data
template <typename _T>
class Stack
{
private:
    Stack<_T> *m_prev = nullptr;
    _T m_data;

public:
    Stack() = default;

    _T data() const     {   return m_data;   }
    Stack<_T>* prev() const {   return m_prev;  }

    static void push(Stack<_T> **pstack, Stack<_T> *new_item, _T data)
    {
        new_item->m_prev = *pstack;
        *pstack = new_item;
        new_item->m_data = data;
    }

    static Stack<_T>* pop(Stack<_T> **pstack)
    {
        Stack<_T> *item = *pstack;
        if (*pstack)    {
            *pstack = (*pstack)->m_prev;
            item->m_prev = nullptr;
        }
        return item;
    }
};

}
#endif

#endif /* __STACK_H__ */
