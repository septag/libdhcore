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


#ifndef __TIMER_H__
#define __TIMER_H__

#include "types.h"
#include "linked-list.h"
#include "core-api.h"
#include "pool-alloc.h"

/**
 * @defgroup timer Timers
 */

/**
 * Basic timer structure, holds data for each timer instance
 * @ingroup timer
 */
struct timer
{
    float t; /**< elapsed time */
    float dt; /**< delta time */
    float rate; /**< playrate, =0.0 stopped, =1.0 normal */
    struct linked_list node; /**< linked-list node */
};

#define TIMER_PAUSE(tm) tm->rate = 0;
#define TIMER_START(tm) tm->rate = 1.0f;
#define TIMER_SCALE(tm, s) tm->rate *= s;
#define TIMER_STOP(tm) tm->rate = 0;    tm->t = 0.0f;   tm->dt = 0.0f;

/* */
result_t timer_initmgr();
void timer_releasemgr();

/**
 * Add timers to the timer system \n
 * Added timers will be updated only after each call to @e timer_update\n
 * To calculate Hi-res timing manually and on-demand, use @e timer_calctm and @e timer_querytick 
 * functions
 * @param start defines if timer should be started immediately after added to the manager
 * @see timer_update
 * @see timer_destroyinstance
 * @see timer_querytick
 * @see timer_calctm
 * @ingroup timer
 */
CORE_API struct timer* timer_createinstance(int start);

/**
 * Remove timer from timer_mgr\n
 * removed timer will no longer be updated
 * @ingroup timer
 */
CORE_API void timer_destroyinstance(struct timer* tm);

/**
 * Update all added timers in timer_mgr
 * @param tick timer tick of current point in time
 * @see timer_querytick
 * @ingroup timer
 */
CORE_API void timer_update(uint64 tick);

/**
 * Query cpu tick time, for manual timing calculation.
 * @return 64bit Integer tick value, which can be passed to @e timer_calctm for actual time calculation
 * @see timer_calctm
 * @ingroup timer
 */
CORE_API uint64 timer_querytick();

/**
 * Update frequency value of timer_mgr
 * @ingroup timer
 */
CORE_API uint64 timer_queryfreq();

/**
 * Calculates the time (in seconds) between two ticks
 * @see timer_querytick
 * @ingroup timer
 */
CORE_API fl64 timer_calctm(uint64 tick1, uint64 tick2);

/**
 * Pause all timers
 * @ingroup timer
 */
CORE_API void timer_pauseall();

/**
 * Resume all timers
 * @ingroup timer
 */
CORE_API void timer_resumeall();

#ifdef __cplusplus

#include "err.h"

namespace dh {

class Timer
{
private:
    timer *m_tm;

public:
    Timer() : m_tm(NULL) {}

    result_t create(bool start = false)
    {
        m_tm = timer_createinstance(start);
        return m_tm != NULL ? RET_OUTOFMEMORY : RET_OK;
    }

    void destroy()
    {
        ASSERT(m_tm);
        timer_destroyinstance(m_tm);
    }

    void start()
    {
        ASSERT(m_tm);
        TIMER_START(m_tm);
    }

    void pause()
    {
        ASSERT(m_tm);
        TIMER_PAUSE(m_tm);
    }

    void stop()
    {
        ASSERT(m_tm);
        TIMER_STOP(m_tm);
    }

    float delta() const
    {
        return m_tm->dt;
    }

    float t() const
    {
        return m_tm->t;
    }

    float rate() const
    {
        return m_tm->rate;
    }

    void set_rate(float rate)
    {
        m_tm->rate = rate;
    }
};

class ProfileTimer
{
private:
    uint64 m_t0;

public:
    ProfileTimer() : m_t0(0) {}

    void begin()
    {
        m_t0 = timer_querytick();
    }

    double end()
    {
        return timer_calctm(m_t0, timer_querytick());
    }
};

}
#endif

#endif /*__TIMER_H__*/
    
