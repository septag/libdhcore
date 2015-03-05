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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"

/**
 * @defgroup queue Queue
 * FIFO (first-in-first-out) queue structure \n
 * Example usage: \n
 * @code
 * // queue must be wrapped inside your own structures/objects
 * struct myobj
 * {
 *     int data;
 *     struct queue node;
 * };
 * 
 * // we hold the master queue (pointer) to keep track of the queue
 * struct queue* root = NULL;
 * 
 * struct myobj qobj1;
 * struct myobj qobj2;
 * memset(&qobj1, 0x00, sizeof(qobj1));
 * memset(&qobj2, 0x00, sizeof(qobj2));
 * 
 * qobj1.data = 1;
 * qobj2.data = 2;
 * 
 * queue_push(&root, &qobj1.node, &qobj1);
 * queue_push(&root, &qobj2.node, &qobj2);
 * 
 * // ...
 * // pop from queue
 * struct queue* qitem;
 * while ((qitem = queue_pop(&root)) != NULL)  {
 *    struct myobj* obj = qitem->data;
 *    printf("data: %d\n", obj->data);
 * }
 * @endcode
 * @see queue_push
 * @see queue_pop
 * @ingroup queue
 */

struct queue
{
    struct queue* next;
    void* data;

#ifdef __cplusplus
    queue() : next(NULL), data(NULL)    {}
#endif
};

/**
 * Push an item into queue
 * @param pqueue pointer to the root queue (can be NULL)
 * @param item queue item to push
 * @param data pointer to owner of the 'item'
 * @ingroup queue
 */
INLINE void queue_push(struct queue** pqueue, struct queue* item, void* data)
{
    if (*pqueue != NULL)    {
        struct queue* last = *pqueue;
        while (last->next != NULL)  
            last = last->next;
        last->next = item;
    }    else    {
        *pqueue = item;
    }
    item->next = NULL;
    item->data = data;
}

/**
 * Pops an item from queue
 * @param pqueue pointer to the root queue item, if the last item is removed *plist will be NULL
 * @return popped queue item, check it's ->data for owner data
 * @ingroup queue
 */
INLINE struct queue* queue_pop(struct queue** pqueue)
{
    struct queue* item = *pqueue;
    if (*pqueue != NULL)    {
        *pqueue = (*pqueue)->next;
        item->next = NULL;
    }
    return item;
}

#ifdef __cplusplus
namespace dh {
template <typename _T>
class Queue
{
private:
    Queue<_T> *m_next = nullptr;
    _T *m_obj = nullptr;

public:
    Queue() = default;
    _T* obj() const     {   return m_obj;   }
    Queue<_T>* next() const {   return m_next;  }

public:
    static void push(Queue<_T> **pqueue, Queue<_T> *new_item, _T *obj)
    {
        if (*pqueue)    {
            Queue<_T> *last = *pqueue;
            while (last->m_next != NULL)
                last = last->m_next;
            last->m_next = new_item;
        }    else    {
            *pqueue = new_item;
        }
        new_item->m_next = nullptr;
        new_item->m_obj = obj;
    }

    static Queue<_T>* pop(Queue<_T> **pqueue)
    {
        Queue<_T> *item = *pqueue;
        if (*pqueue)    {
            *pqueue = (*pqueue)->m_next;
            item->m_next = nullptr;
        }
        return item;
    }
};

}
#endif

#endif /* __QUEUE_H__ */
