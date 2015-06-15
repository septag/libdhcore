/***********************************************************************************
 * Copyright (c) 2013, Davide Bacchet
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

#if defined(_APPLE_)

#include <mach/mach_time.h>

static struct mach_timebase_info g_freq;

void timer_queryfreq()
{
    mach_timebase_info(&g_freq);
}

uint64 timer_querytick()
{
    return mach_absolute_time();
}

fl64 timer_calctm(uint64 tick1, uint64 tick2)
{
    uint64 delta_tick = tick2 - tick1;
    uint64 utm = delta_tick*g_freq.numer/g_freq.denom;
    return (double)utm/1e9;
}

#endif /* _OSX_ */
