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

#include "dhcore/timer.h"
#include "dhcore/mem-mgr.h"
#include "dhcore/err.h"

 /* types */
struct timer_mgr
{
    uint64 freq;
    uint64 prev_tick;
    float scale;
    struct pool_alloc timer_pool;
    struct linked_list* timers;
};

/*************************************************************************************************
 * globals
 */
static struct timer_mgr* g_tm = NULL;

/*************************************************************************************************/
result_t timer_initmgr()
{
    if (g_tm != NULL)
        return RET_FAIL;
    g_tm = (struct timer_mgr*)ALLOC(sizeof(struct timer_mgr), 0);
    if (g_tm == NULL)
        return RET_OUTOFMEMORY;
    memset(g_tm, 0x00, sizeof(struct timer_mgr));

    g_tm->scale = 1.0f;
    g_tm->freq = timer_queryfreq();

    /* memory pool for timers */
    return mem_pool_create(mem_heap(), &g_tm->timer_pool, sizeof(struct timer), 20, 0);
}

void timer_releasemgr()
{
    if (g_tm != NULL)   {
        mem_pool_destroy(&g_tm->timer_pool);
        FREE(g_tm);
        g_tm = NULL;
    }
}

struct timer* timer_createinstance(int start)
{
    struct timer* tm = (struct timer*)mem_pool_alloc(&g_tm->timer_pool);
    ASSERT(tm != NULL);
    memset(tm, 0x00, sizeof(struct timer));

    list_add(&g_tm->timers, &tm->node, tm);
    if (start)
        tm->rate = 1.0f;

    return tm;
}

void timer_destroyinstance(struct timer* tm)
{
    list_remove(&g_tm->timers, &tm->node);
    memset(tm, 0x00, sizeof(struct timer));
    mem_pool_free(&g_tm->timer_pool, tm);
}

void timer_update(uint64 tick)
{
    if (g_tm->prev_tick == 0)
        g_tm->prev_tick = tick;

    fl64 dt = ((fl64)(tick - g_tm->prev_tick)) / ((fl64)g_tm->freq);
    dt *= g_tm->scale;
    g_tm->prev_tick = tick;
    float dtf = (float)dt;

    /* move through the linked-list of timers and update them */
    struct linked_list* tm_node = g_tm->timers;
    while (tm_node != NULL)     {
        struct timer* tm = (struct timer*)tm_node->data;
        tm->dt = dtf * tm->rate;
        tm->t += tm->dt;
        tm_node = tm_node->next;
    }
}

fl64 timer_calctm(uint64 tick1, uint64 tick2)
{
    fl64 freq = (fl64)g_tm->freq;
    fl64 dt = (fl64)((int64)(tick2 - tick1));
    return dt / freq;
}

void timer_pauseall()
{
    struct linked_list* tm_node = g_tm->timers;
    while (tm_node != NULL)     {
        struct timer* tm = (struct timer*)tm_node->data;
        TIMER_PAUSE(tm);
        tm_node = tm_node->next;
    }
}

void timer_resumeall()
{
    struct linked_list* tm_node = g_tm->timers;
    while (tm_node != NULL)     {
        struct timer* tm = (struct timer*)tm_node->data;
        TIMER_START(tm);
        tm_node = tm_node->next;
    }
}
