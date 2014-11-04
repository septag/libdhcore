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

#include <stdio.h>
#include <stdlib.h>

#include "dhcore/core.h"
#include "dhcore/commander.h"

#include "dhcore-test.h"

typedef void (*pfn_test)();

struct unit_test_desc
{
    pfn_test test_fn;
    const char* name;
    const char* desc;
};

static const struct unit_test_desc g_tests[] = {
    {test_heap, "heap", "Heap allocation"},
    {test_freelist, "freelist", "Freelist allocator"},
    {test_json, "json", "JSON parser"},
    {test_mempool, "pool", "Pool allocator"},
    {test_thread, "thread", "Basic threads"},
    {test_taskmgr, "taskmgr", "Task manager"},
    {test_hashtable, "hashtable_fixed", "Hash tables (fixed)"}
    /*, {test_efsw, "watcher", "filesystem monitoring"}*/
};

static int g_testidx = -1;

/* */
void cmd_gettest(command_t* cmd, void* param)
{
    if (str_isequal_nocase(cmd->arg, "heap"))   {
        g_testidx = 0;
    }   else if (str_isequal_nocase(cmd->arg, "freelist"))  {
        g_testidx = 1;
    }   else if (str_isequal_nocase(cmd->arg, "json"))  {
        g_testidx = 2;
    }   else if (str_isequal_nocase(cmd->arg, "pool"))  {
        g_testidx = 3;
    }   else if (str_isequal_nocase(cmd->arg, "thread"))  {
        g_testidx = 4;
    }   else if (str_isequal_nocase(cmd->arg, "taskmgr"))  {
        g_testidx = 5;
    }   else if (str_isequal_nocase(cmd->arg, "hashtable")) {
        g_testidx = 6;
    }
}

int show_help()
{
    printf("Choose unit test: \n");
    uint test_cnt = sizeof(g_tests)/sizeof(struct unit_test_desc);

    for (uint i = 0; i < test_cnt; i++)  {
        printf("%d- %s (%s)\n", i, g_tests[i].desc, g_tests[i].name);
    }
    printf("q- quit\n");

    char r = util_getch();
    if (r == 'q')   {
        return -1;
    }   else if (r >= '0' && r <= '9')   {
        char rs[2];
        rs[0] = (char)r;
        rs[1] = 0;
        return atoi(rs);
    }   else    {
        return -1;
    }
}

int parse_cmd(const char* arg)
{
    uint test_cnt = sizeof(g_tests)/sizeof(struct unit_test_desc);
    for (uint i = 0; i < test_cnt; i++)     {
        if (str_isequal_nocase(arg, g_tests[i].name))   {
            return i;
        }
    }
    return -1;
}

int main(int argc, char** argv)
{
    command_t cmd;
    command_init(&cmd, "dhcore-test", "1.0");    
    command_option_pos(&cmd, "test", "Choose unit test", TRUE, cmd_gettest);
    command_parse(&cmd, argc, argv, NULL);

    if (IS_FAIL(core_init(CORE_INIT_ALL)))     {
        printf("core init error.\n");
        return -1;
    }

    log_outputconsole(TRUE);

    if (g_testidx == -1)
        g_testidx = show_help();

    if (g_testidx != -1) 
        g_tests[g_testidx].test_fn();

#if defined(_DEBUG_)
    core_release(TRUE);
#else
    core_release(FALSE);
#endif
    return 1;
}
