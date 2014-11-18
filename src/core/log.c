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


#if defined(_WIN_)
#include "dhcore/win.h"
#endif

#include <stdio.h>
#include <stdarg.h>

#include "dhcore/mem-mgr.h"
#include "dhcore/log.h"
#include "dhcore/mt.h"
#include "dhcore/util.h"
#include "dhcore/str.h"

#define LINE_COUNT_FLUSH    20

/* fwd declarations */
static void log_outputtext(enum log_type type, const char* text);

/* types */
struct log_mgr
{
    struct log_stats stats;
    uint outputs;
    char log_filepath[DH_PATH_MAX];
    FILE* log_file;
    pfn_log_handler log_fn;
    void* fn_param;

#ifdef _WIN_
    HANDLE con_hdl;
    WORD con_attrs;
#endif
};

enum output_mode
{
    OUTPUT_CONSOLE = (1<<0),
    OUTPUT_DEBUGGER = (1<<1),
    OUTPUT_FILE = (1<<2),
    OUTPUT_CUSTOM = (1<<3)
};

/* globals */
static struct log_mgr* g_log = NULL;

/*************************************************************************************************/
result_t log_init()
{
    if (g_log != NULL)
        return RET_FAIL;
    g_log = (struct log_mgr*)ALLOC(sizeof(struct log_mgr), 0);
    if (g_log == NULL)
        return RET_OUTOFMEMORY;
    memset(g_log, 0x00, sizeof(struct log_mgr));

    return RET_OK;
}

void log_release()
{
    if (g_log != NULL)  {
        if (g_log->log_file != NULL)
            fclose(g_log->log_file);

        FREE(g_log);
        g_log = NULL;
    }
}

result_t log_outputconsole(int enable)
{
    if (enable)
    	BIT_ADD(g_log->outputs, OUTPUT_CONSOLE);
    else
    	BIT_REMOVE(g_log->outputs, OUTPUT_CONSOLE);

#ifdef _WIN_
    if (enable) {
        g_log->con_hdl = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO con_info;
        GetConsoleScreenBufferInfo(g_log->con_hdl, &con_info);
        g_log->con_attrs = con_info.wAttributes;
    }   else    {
        if (g_log->con_hdl) {
            SetConsoleTextAttribute(g_log->con_hdl, g_log->con_attrs);
            CloseHandle(g_log->con_hdl);
        }
    }
#endif
    return RET_OK;
}

result_t log_outputfile(int enable, const char* log_filepath)
{
    BIT_ADD(g_log->outputs, OUTPUT_FILE);

    /* if logfile is previously opened, close it */
    if (g_log->log_file != NULL)     {
        fclose(g_log->log_file);
        g_log->log_file = NULL;
        BIT_REMOVE(g_log->outputs, OUTPUT_FILE);
    }

    if (enable)     {
        g_log->log_file = fopen(log_filepath, "wt");
        if (g_log->log_file == NULL)
            return RET_FILE_ERROR;

        BIT_ADD(g_log->outputs, OUTPUT_FILE);
        strcpy(g_log->log_filepath, log_filepath);
    }

    return RET_OK;
}

result_t log_outputdebugger(int enable)
{
    if (enable)
    	BIT_ADD(g_log->outputs, OUTPUT_DEBUGGER);
    else
    	BIT_REMOVE(g_log->outputs, OUTPUT_DEBUGGER);

    return RET_OK;
}

result_t log_outputfunc(int enable, pfn_log_handler log_fn, void* param)
{
    if (enable)     {
        BIT_ADD(g_log->outputs, OUTPUT_CUSTOM);
        g_log->log_fn = log_fn;
        g_log->fn_param = param;
    }    else       {
        BIT_REMOVE(g_log->outputs, OUTPUT_CUSTOM);
        g_log->log_fn = NULL;
        g_log->fn_param = NULL;
    }

    return RET_OK;
}

int log_isconsole()
{
    return BIT_CHECK(g_log->outputs, OUTPUT_CONSOLE);
}

int log_isfile()
{
    return BIT_CHECK(g_log->outputs, OUTPUT_FILE);
}

int log_isdebugger()
{
    return BIT_CHECK(g_log->outputs, OUTPUT_DEBUGGER);
}

int log_isoutputfunc()
{
    return BIT_CHECK(g_log->outputs, OUTPUT_CUSTOM);
}

void log_print(enum log_type type, const char* text)
{
    log_outputtext(type, text);
}

void log_printf(enum log_type type, const char* fmt, ...)
{
    char text[2048];
    text[0] = 0;

    va_list args;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);

    log_outputtext(type, text);
}

void log_getstats(struct log_stats* stats)
{
    memcpy(stats, &g_log->stats, sizeof(g_log->stats));
}

void log_endprogress(enum log_progress_result res)
{
#ifndef _WIN_
    const char *text;

    switch (res)    {
    case LOG_PROGRESS_OK:
        text = "[" TERM_GREEN "OK" TERM_RESET "]";
        break;
    case LOG_PROGRESS_FATAL:
        text = "[" TERM_BOLDRED "FAILED" TERM_RESET "]";
        break;
    case LOG_PROGRESS_NONFATAL:
        text = "[" TERM_BOLDYELLOW "FAILED" TERM_RESET "]";
        break;
    default:
        text = "";
        break;
    }
    log_outputtext(LOG_PROGRESS_RESULT, text);
#endif
}

static void log_outputtext(enum log_type type, const char* text)
{
    const char* prefix;
    char msg[2048];

    switch (type)   {
        case LOG_ERROR:
            prefix = "";
            MT_ATOMIC_INCR(g_log->stats.errors_cnt);
            break;

        case LOG_WARNING:
            prefix = "";
            MT_ATOMIC_INCR(g_log->stats.warnings_cnt);
            break;

        case LOG_LOAD:
            prefix = "Load: ";
            break;

        default:
            prefix = "";
            break;
    }

    MT_ATOMIC_INCR(g_log->stats.msgs_cnt);
    strcpy(msg, prefix);
    str_safecat(msg, sizeof(msg)-1, text);

    /* message is ready, dispatch it to outputs */
    if (BIT_CHECK(g_log->outputs, OUTPUT_CONSOLE))   {
#if !defined(_WIN_)        
        char msg2[2060];
        const char* color;
        switch (type)   {
        case LOG_ERROR:
            color = TERM_RED;
            break;
        case LOG_WARNING:
            color = TERM_YELLOW;
            break;
        case LOG_INFO:
            color = TERM_DIM;
            break;
        case LOG_LOAD:
            color = TERM_DIMCYAN;
            break;
        case LOG_TEXT:
        case LOG_PROGRESS:
        case LOG_PROGRESS_RESULT:
            color = TERM_RESET;
            break;
        default:
            color = "";
        }

        strcpy(msg2, color);
        strcat(msg2, msg);
        strcat(msg2, TERM_RESET);

        if (type != LOG_PROGRESS)
            puts(msg2);
        else if (type == LOG_PROGRESS_RESULT)
            puts(msg2);
        else
            printf("%-" LOG_STDOUT_PADDING "s", msg2);
#else
        ASSERT(g_log->con_hdl);
        switch (type)   {
        case LOG_ERROR:
            SetConsoleTextAttribute(g_log->con_hdl, FOREGROUND_RED|FOREGROUND_INTENSITY);
            break;
        case LOG_WARNING:
            SetConsoleTextAttribute(g_log->con_hdl, FOREGROUND_RED|FOREGROUND_GREEN);
            break;
        case LOG_TEXT:
        case LOG_PROGRESS:
            SetConsoleTextAttribute(g_log->con_hdl, 
                FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
            break;
        case LOG_PROGRESS_RESULT:
        case LOG_INFO:
            SetConsoleTextAttribute(g_log->con_hdl, g_log->con_attrs);
            break;
        case LOG_LOAD:
            SetConsoleTextAttribute(g_log->con_hdl, FOREGROUND_BLUE);
            break;
        }
        puts(msg);
        SetConsoleTextAttribute(g_log->con_hdl, g_log->con_attrs);
#endif
    }
    
    
    if (type != LOG_PROGRESS)
        strcat(msg, "\n");

    if (BIT_CHECK(g_log->outputs, OUTPUT_FILE))  {
        fputs(msg, g_log->log_file);
        fflush(g_log->log_file);
    }

#if defined(_MSVC_) && defined(_DEBUG_)
    if (BIT_CHECK(g_log->outputs, OUTPUT_DEBUGGER))
        OutputDebugString(msg);
#endif

    if (BIT_CHECK(g_log->outputs, OUTPUT_CUSTOM))
        g_log->log_fn(type, msg, g_log->fn_param);
}
