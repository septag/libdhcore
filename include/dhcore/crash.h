/***********************************************************************************
 * Copyright (c) 2013, Sepehr Taghdisian
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

#ifndef __CRASH_H__
#define __CRASH_H__

#include "types.h"
#include "core-api.h"

/**
 * Crash handler callback function
 * @ingroup core
 */
typedef void (*pfn_crash_handler)();

/* */
result_t crash_init();

/**
 * Sets crash handler callback function, so when application crashes, the callback is called.
 * @ingroup core
 */
CORE_API void crash_set_handler(pfn_crash_handler crash_fn);

#endif /* __CRASH_H__ */