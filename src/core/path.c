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
#include <stdarg.h>

#include "dhcore/path.h"

#if defined(_WIN_)
#include "dhcore/win.h"
#include <Shlwapi.h>
#endif

#ifdef _POSIXLIB_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(_WIN_)
#define SEP_CHAR '\\'
#else
#define SEP_CHAR '/'
#endif

char* path_norm(char* outpath, const char* inpath)
{
    if (inpath[0] == 0) {
        outpath[0] = 0;
        return outpath;
    }

#if defined(_WIN_)
    char tmp[DH_PATH_MAX];
    GetFullPathName(inpath, DH_PATH_MAX, tmp, NULL);
    path_towin(outpath, tmp);
    size_t sz = strlen(outpath);
    if (outpath[sz-1] == '\\')
        outpath[sz-1] = 0;
    return outpath;
#else

    char* tmp = realpath(inpath, NULL);
    if (tmp != NULL)    {
        path_tounix(outpath, tmp);
        free(tmp);
    }   else    {
        char tmp2[DH_PATH_MAX];
        realpath(inpath, tmp2);
        strcpy(outpath, tmp2);
    }

    size_t sz = strlen(outpath);
    if (outpath[sz-1] == '/')
        outpath[sz-1] = 0;
    return outpath;
#endif
}

char* path_tounix(char* outpath, const char* inpath)
{
    char tmp[DH_PATH_MAX];
    strcpy(tmp, inpath);
    str_replace(tmp, '\\', '/');
    strcpy(outpath, tmp);
    return outpath;
}

char* path_towin(char* outpath, const char* inpath)
{
    char tmp[DH_PATH_MAX];
    strcpy(tmp, inpath);
    str_replace(tmp, '/', '\\');
    strcpy(outpath, tmp);
    return outpath;
}

char* path_getdir(char* outpath, const char* inpath)
{
    /* to prevent aliasing */
    char tmp[DH_PATH_MAX];
    strcpy(tmp, inpath);

    /* Path with '/' or '\\' at the End */
    char* r = strrchr(tmp, '/');
    if (r == NULL)     r = strrchr(tmp, '\\');
    if (r != NULL)     {    strncpy(tmp, inpath, (r - tmp)); tmp[r - tmp] = 0;    }
    else               tmp[0] = 0;

    strcpy(outpath, tmp);
    return outpath;
}

char* path_getfilename(char* outpath, const char* inpath)
{
    char* r;
    char tmp[DH_PATH_MAX];
    strcpy(tmp, inpath);

    r = strrchr(tmp, '/');
    if (r == NULL)     r = strrchr(tmp, '\\');
    if (r != NULL)     strcpy(tmp, r + 1);

    /* Name only */
    r = strrchr(tmp, '.');
    if (r != NULL)     *r = 0;

    strcpy(outpath, tmp);
    return outpath;
}

char* path_getfileext(char* outpath, const char* inpath)
{
    char tmp[DH_PATH_MAX];     /* Prevent Aliasing */

    strcpy(tmp, inpath);
    char* r = strrchr(tmp, '.');
    if (r != NULL)     strcpy(tmp, r + 1);
    else               tmp[0] = 0;

    r = strchr(tmp, '/');

    strcpy(outpath, (r != NULL) ? (r + 1) : tmp);
    return outpath;
}

char* path_getfullfilename(char* outpath, const char* inpath)
{
    const char* r;
    char tmp[DH_PATH_MAX];     /* Prevent Aliasing */
    strcpy(tmp, inpath);
    r = strrchr(inpath, '/');
    if (r == NULL)     r = strrchr(inpath, '\\');
    if (r != NULL)     strcpy(tmp, r + 1);
    else               strcpy(tmp, inpath);

    strcpy(outpath, tmp);
    return outpath;
}

char* path_goup(char* outpath, const char* inpath)
{
    char tmp[DH_PATH_MAX];
    strcpy(tmp, inpath);
    size_t s = strlen(tmp);

    if (tmp[s-1] == '/' || tmp[s-1] == '\\')
        tmp[s-1] = 0;

    /* handle case when the path is like 'my/path/./' */
    if (s>3 && tmp[s-2] == '.' && (tmp[s-3] == '/' || tmp[s-3] == '\\'))
        tmp[s-3] = 0;
    /* handle case when the path is like 'my/path/.' */
    if (s>2 && tmp[s-1] == '.' && (tmp[s-2] == '/' || tmp[s-2] == '\\'))
        tmp[s-2] = 0;

    char* up = strrchr(tmp, '/');
    if (up == NULL)
        up = strrchr(tmp, '\\');

    if (up != NULL)
        *up = 0;

    strcpy(outpath, tmp);
    return outpath;
}

int path_exists(const char* inpath)
{
#ifdef _POSIXLIB_
    struct stat s;
    if (stat(inpath, &s) == 0)    {
        if (s.st_mode & S_IFDIR)
            return 2;
        else if ((s.st_mode & (S_IFREG|S_IFLNK)))
            return 1;
        else
            return 0;

    }   else    {
        return FALSE;
    }
#elif _WIN_
    if (PathIsDirectory(inpath))
        return 2;
    else if (PathFileExists(inpath))
        return 1;
    else
        return 0;
#endif
}

char* path_join(char* outpath, const char* join0, const char* join1, ...)
{
    char tmp[DH_PATH_MAX];
    char sep[] = {SEP_CHAR, 0};

    if (join0[0] != 0)   {
        strcpy(tmp, join0);
        strcat(tmp, sep);
        strcat(tmp, join1);
    }   else    {
        strcpy(tmp, join1);
    }

    va_list args;
    va_start(args, join1);
    const char* join2;
    while ((join2 = va_arg(args, const char*)) != NULL) {
        strcat(tmp, sep);
        strcat(tmp, join2);
    }
    va_end(args);

    return strcpy(outpath, tmp);
}
