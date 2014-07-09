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

#ifndef PATH_H
#define PATH_H

#include "types.h"
#include "core-api.h"
#include "str.h"

/**
 * convert windows-style path ("\\t\\win\\path") to unix-style path ("/t/win/path")
 * @ingroup str
 */
CORE_API char* path_tounix(char* outpath, const char* inpath);
/**
 * convert unix-style path ("/t/unix/path") to windows-style path ("\\t\\unix\\path")
 * @ingroup str
 */
CORE_API char* path_towin(char* outpath, const char* inpath);
/**
 * convert path to platform specific format
 * @ingroup str
 */
CORE_API char* path_norm(char* outpath, const char* inpath);
/**
 * extract directory from the path
 * @ingroup str
 */
CORE_API char* path_getdir(char* outpath, const char* inpath);
/**
 * extract filename from the path, without any extensions
 * @ingroup str
 */
CORE_API char* path_getfilename(char* outpath, const char* inpath);
/**
 * extract file extension from the path
 * @ingroup str
 */
CORE_API char* path_getfileext(char* outpath, const char* inpath);
/**
 * extract full filename (with extension) from path
 * @ingroup str
 */
CORE_API char* path_getfullfilename(char* outpath, const char* inpath);
/**
 * go up one directory in path string
 * @ingroup str
 */
CORE_API char* path_goup(char* outpath, const char* inpath);
/**
 * check if file is valid (exists)
 * @return returns 0 if nothing exists, 1 if it's a file, and 2 if directory (unix only)
 * @ingroup str
 */
CORE_API int path_exists(const char* inpath);

/**
 * join multiple paths (or filenames) into one \n
 * Last argument should always be NULL to indicate that join arguments are finished
 * @ingroup str
 */
CORE_API char* path_join(char* outpath, const char* join0, const char* join1, ...);

#endif // PATH_H
