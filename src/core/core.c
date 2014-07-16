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

#include "core.h"
#include "json.h"
#include "file-io.h"
#include "timer.h"
#include "crash.h" 
#include "net-socket.h"

#ifdef _DEBUG_
  #include <stdio.h>
#endif

result_t core_init(uint flags)
{
    char msg[128];
    snprintf(msg, sizeof(msg), TERM_RESET "Initializing Core library v%s" TERM_RESET, _VERSION_);
    printf("%-" LOG_STDOUT_PADDING "s", msg);
    if (BIT_CHECK(flags, CORE_INIT_CRASHDUMP))  {
        if (IS_FAIL(crash_init()))  {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    if (IS_FAIL(mem_init(BIT_CHECK(flags, CORE_INIT_TRACEMEM))))    {
        puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
        return RET_FAIL;
    }

    if (IS_FAIL(log_init()))    {
        puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
        return RET_FAIL;
    }

    if (BIT_CHECK(flags, CORE_INIT_ERRORS)) {
        if (IS_FAIL(err_init()))    {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    rand_seed();

    if (BIT_CHECK(flags, CORE_INIT_JSON))   {
        if (IS_FAIL(json_init()))   {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    if (BIT_CHECK(flags, CORE_INIT_FILEIO)) {
        if (IS_FAIL(fio_initmgr())) {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    if (BIT_CHECK(flags, CORE_INIT_TIMER)) {
        if (IS_FAIL(timer_initmgr()))   {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    if (BIT_CHECK(flags, CORE_INIT_SOCKET)) {
        if (IS_FAIL(sock_init()))   {
            puts("[" TERM_BOLDRED "FAILED" TERM_RESET "]");
            return RET_FAIL;
        }
    }

    puts("[" TERM_GREEN "OK" TERM_RESET "]");
    return RET_OK;
}

void core_release(int report_leaks)
{
    printf("%-" LOG_STDOUT_PADDING "s", TERM_RESET "Releasing Core library" TERM_RESET);

    sock_release();

	timer_releasemgr();

    fio_releasemgr();

    json_release();

    err_release();

    log_release();

    puts("[" TERM_GREEN "OK" TERM_RESET "]");

    /* dump memory leaks before releasing memory manager and log
     * because memory leak report dumps leakage data to logger */
    if (report_leaks)
        mem_reportleaks();
    
    mem_release();
}
