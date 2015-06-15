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

#if defined(_WIN_)

#include "dhcore/win.h"

static LONG_INTEGER g_freq;

void timer_queryfreq()
{
    QueryPerformanceFrequency(&g_freq);
}

uint64 timer_querytick()
{
    LARGE_INTEGER tick;
    QueryPerformanceCounter(&tick);
    return (uint64)tick.QuadPart;
}

fl64 timer_calctm(uint64 tick1, uint64 tick2)
{
    uint64 delta_tick = tick2 - tick1;
    return (double)delta_tick/(double)g_freq.QuadPart;
}

#endif /* _WIN_ */
