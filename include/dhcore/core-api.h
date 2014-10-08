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


#ifndef __COREAPI_H__
#define __COREAPI_H__

#include "types.h"

#ifndef _CORE_STATICLIB_
  #if defined(_CORE_EXPORT_)
    #if defined(_MSVC_)
      #define CORE_API _EXTERN_EXPORT_ __declspec(dllexport)
    #elif defined(_GNUC_)
      #define CORE_API _EXTERN_EXPORT_ __attribute__((visibility("default")))
    #endif
  #else
    #if defined(_MSVC_)
   	  #define CORE_API _EXTERN_EXPORT_ __declspec(dllimport)
    #elif defined(_GNUC_)
      #define CORE_API _EXTERN_EXPORT_ __attribute__((visibility("default")))
    #endif
  #endif /* defined(_CORE_EXPORT) */
#else
  #define CORE_API _EXTERN_EXPORT_
#endif

#if defined(SWIG)
#define CORE_API
#endif

#endif /* __COREAPI_H__*/
