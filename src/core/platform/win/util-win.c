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

#include "dhcore/util.h"

#if defined(_WIN_)

#include "dhcore/win.h"
#include <conio.h>
#include <Shlobj.h>

#include "dhcore/err.h"
#include "dhcore/str.h"
#include "dhcore/path.h"

char* util_getconfdir(char *outdir)
{
    return path_join(outdir, util_getuserdir(outdir), "AppData", "Local", NULL);
}

char* util_getexedir(char* outpath)
{
    GetModuleFileName(NULL, outpath, DH_PATH_MAX);
    path_getdir(outpath, outpath);
    return path_norm(outpath, outpath);
}

char* util_runcmd(const char* cmd)
{
    ASSERT(0);
    return NULL;
}

char util_getch()
{
    return _getch();
}

char* util_getuserdir(char* outdir)
{
    outdir[0] = 0;
    SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, outdir);
    return outdir;
}

char* util_gettempdir(char* outdir)
{
    GetTempPath(DH_PATH_MAX, outdir);

    /* remove \\ from the end of the path */
    size_t sz = strlen(outdir);
    if (sz > 0 && outdir[sz-1] == '\\')
        outdir[sz-1] = 0;
    return outdir;
}

int util_makedir(const char* dir)
{
    return CreateDirectory(dir, NULL);
}

int util_copyfile(const char* dest, const char* src)
{
    return CopyFile(src, dest, FALSE);
}

int util_pathisdir(const char* path)
{
    DWORD atts = GetFileAttributes(path);
    if (atts == INVALID_FILE_ATTRIBUTES)
        return FALSE;
    return (atts & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
}

void util_sleep(uint msecs)
{
    Sleep(msecs);
}

int util_movefile(const char* dest, const char* src)
{
    return MoveFileEx(src, dest, MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING);
}

int util_delfile(const char* filepath)
{
    return DeleteFile(filepath);
}

#endif /* _WIN_ */
