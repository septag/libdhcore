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


#ifndef __TYPES_H__
#define __TYPES_H__

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "error-codes.h"

/* Compiler macros
 * _MSVC_: Microsoft visual-c compiler
 * _GNUC_: GNUC compiler
 */
#if _MSC_VER
  #define _MSVC_
  #pragma warning(disable:4800) // forcing int to bool
#endif

#if __GNUC__
  #define _GNUC_
#endif

// Debug
#ifndef _DEBUG_
  #if defined(_DEBUG) || defined(DEBUG) || defined(__Debug__)
    #define _DEBUG_
  #endif
#endif

// Windows
#ifndef _WIN_
  #if defined(WIN32) || defined(_WINDLL)
    #define _WIN_
  #endif

  #if defined(SWIG) && defined(SWIGWIN)
    #define _WIN_
  #endif
#endif

// Apple iOS/MacOS
#ifdef __APPLE__
  #define _APPLE_
  #include <TargetConditionals.h>
  #if TARGET_OS_MAC == 1 && TARGET_OS_IPHONE == 0 && !defined(_OSX_)
    #define _OSX_
  #elif TARGET_OS_IPHONE == 1 && !defined(_IOS_)
    #define _IOS_
  #endif
#endif

// Linux
#ifndef _LINUX_
  #if defined(linux) || defined(__linux__)
    #define _LINUX_
  #endif
#endif

// Can override default alignment for allocation macros
#ifndef _ALIGN_DEFAULT_
  #define _ALIGN_DEFAULT_ 16
#endif

// Error on unsupported compilers
#if !defined(_MSVC_) && !defined(_GNUC_) && !defined(SWIG)
  #error "Compile Error: Unsupported compiler."
#endif

// Error on unsupported platforms
#if !defined(_WIN_) && !defined(_LINUX_) && !defined(_OSX_) && !defined(_IOS_)
  #error "Compiler error: Unsupported software platform."
#endif

// Group Mobile OSes together
#ifndef _MOBILE_
  #if defined(_IOS_) || defined(_ANDROID_) || defined(_WINPHONE_)
    #define _MOBILE_
  #endif
#endif

// CPU: Intel x86_64 family
#ifndef _X86_64
  #if defined(_M_IX86) || defined(__i386__) || defined(__X86_64__) || defined(_M_X64) || defined(__x86_64__)
    #define _X86_64_
  #endif
#endif

// CPU: ARM
#ifndef _ARM_
  #if defined(__arm__) || defined(__arm64__)
    #define _ARM_
    #if defined(__ARM_ARCH) && __ARM_ARCH==6
      #define _ARMv6_
    #endif
  #endif
#endif

// 32bit/64bit
#if !defined(_ARCH64_) || !defined(_ARCH32_)
  #if defined(_M_X64) || defined(__X86_64__) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
    #define _ARCH64_
  #else
    #define _ARCH32_
  #endif
#endif

#if (!defined(_X86_64_) && !defined(_ARM_)) && !defined(SWIG)
  #error "CPU architecture is unknown"
#endif

// POSIX
#ifndef _POSIXLIB_
  #if defined(__linux__) || defined(__APPLE__)
    #define _POSIXLIB_
  #endif
#endif

/* On the default msvc compiler, I use /TP flag which compiles all files as CPP
 * But under ICC we use the common C99 feature (compile in C)
 * So this macro is defined for msvc compiler only, so we can ignore extern "C" in headers
 * On my compiler, only MSVC x64 compiler rasises extern "C" symbol errors (not the x86 one).
 */
#if defined(_MSVC_) && !defined(__INTEL_COMPILER)
  #define _CPP_IS_FORCED_
#endif

#if defined(__cplusplus)
  #if !defined(_CPP_IS_FORCED_)
    #define _EXTERN_ extern "C"
    #define _EXTERN_BEGIN_ extern "C" {
    #define _EXTERN_END_ }
  #else
    #define _EXTERN_
    #define _EXTERN_BEGIN_
    #define _EXTERN_END_
  #endif
  #define _EXTERN_EXPORT_ extern "C" 
#else
  #define _EXTERN_
  #define _EXTERN_BEGIN_
  #define _EXTERN_END_
  #define _EXTERN_EXPORT_
#endif

/* common data-type defs */
typedef int int32;
typedef long long int int64;
typedef unsigned long long int uint64;
typedef unsigned int uint;

#ifndef DHCORE_IGNORE_BASIC_TYPEDEFS
typedef char int8;
typedef short int16;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
#endif

typedef wchar_t wchar;
typedef float fl32;
typedef double fl64;
typedef int result_t;
typedef uint64 reshandle_t;

/* pointer type (64/32 platforms are different) */
#if defined(_ARCH64_)
typedef uint64 uptr_t;
typedef int64 iptr_t;
#else
typedef uint uptr_t;
typedef int iptr_t;
#endif

/* TRUE/FALSE definitions */
#ifndef TRUE
  #define TRUE    1
#endif

#ifndef FALSE
  #define FALSE   0
#endif

/* NULL */
#ifndef NULL
  #define NULL    0x0
#endif

/* maximum/minimum definitions */
#if !defined(_WIN_)
  #if defined(UINT64_MAX)
    #undef UINT64_MAX
  #endif
  #if defined(UINT64_MAX)
    #undef UINT64_MAX
  #endif
  #if defined(UINT64_MAX)
    #undef UINT64_MAX
  #endif
  #if defined(UINT32_MAX)
    #undef UINT32_MAX
  #endif
  #if defined(UINT16_MAX)
    #undef UINT16_MAX
  #endif
  #if defined(UINT8_MAX)
    #undef UINT8_MAX
  #endif
  #if defined(INT64_MAX)
    #undef INT64_MAX
  #endif
  #if defined(INT64_MAX)
    #undef INT64_MAX
  #endif
  #if defined(INT64_MAX)
    #undef INT64_MAX
  #endif
  #if defined(INT32_MAX)
    #undef INT32_MAX
  #endif
  #if defined(INT16_MAX)
    #undef INT16_MAX
  #endif
  #if defined(INT8_MAX)
    #undef INT8_MAX
  #endif
  #if defined(INT64_MIN)
    #undef INT64_MIN
  #endif
  #if defined(INT64_MIN)
    #undef INT64_MIN
  #endif
  #if defined(INT64_MIN)
    #undef INT64_MIN
  #endif
  #if defined(INT32_MIN)
    #undef INT32_MIN
  #endif
  #if defined(INT16_MIN)
    #undef INT16_MIN
  #endif
  #if defined(INT8_MIN)
    #undef INT8_MIN
  #endif

  #define UINT64_MAX              (0xffffffffffffffff)
  #define UINT32_MAX              (0xffffffff)
  #define UINT16_MAX              (0xffff)
  #define UINT8_MAX               (0xff)
  #define INT64_MAX               (9223372036854775807)
  #define INT64_MIN               (-9223372036854775807-1)
  #define INT32_MAX               (2147483647)
  #define INT32_MIN               (-2147483647-1)
  #define INT16_MAX               (32767)
  #define INT16_MIN               (-32768)
  #define INT8_MAX                127
  #define INT8_MIN                -127
#else
  #define __STDC_LIMIT_MACROS
  #include <stdint.h>
#endif

#define FL64_MAX                (1.7976931348623158e+308)
#define FL64_MIN                (2.2250738585072014e-308)
#define FL32_MAX                (3.402823466e+38f)
#define FL32_MIN                (1.175494351e-38f)

/* Version info structure, mostly used in file formats */
/* useful macros */
#define INVALID_HANDLE      0xffffffffffffffff

/* inlining */
#if defined(_MSVC_)
  #define INLINE          inline
  #define FORCE_INLINE    INLINE
#endif

#if defined(_GNUC_)
  #define INLINE          static inline
  #define FORCE_INLINE    INLINE
#endif

/* bitwise operators */
#define BIT_CHECK(v, b)     (((v)&(b)) != 0)
#define BIT_ADD(v, b)       ((v) |= (b))
#define BIT_REMOVE(v, b)    ((v) &= ~(b))

/* out/in/INOUT for readability */
#ifndef OUT
  #define OUT
#endif

#ifndef OPTIONAL
  #define OPTIONAL
#endif

#ifndef INOUT
  #define INOUT
#endif

#if defined(_GNUC_)
  #define DEF_ALLOC inline
#else
  #define DEF_ALLOC INLINE
#endif

/* maximum path string length */
#define DH_PATH_MAX  255

#ifndef INVALID_INDEX
  #define INVALID_INDEX 0xffffffff
#endif

#endif /* __TYPES_H__ */
