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

#ifndef __TASKMGR_H__
#define __TASKMGR_H__

#include "types.h"
#include "allocator.h"
#include "core-api.h"

/**
 * @defgroup taskman Task manager
 * Multi-threaded task dispatcher, basic idea is that you implement a callback for running a task in 
 * threads, and call dispatch to run it in multiple threads.\n
 * Example Usage: \n
 * @code
 * // setup task manager to support up to 4 threads, without any temp or local memory allocators
 * int myarray1[100];
 * int myarray2[100];
 * int myarray_result[100];
 * const uint dispatch_count = 4;
 * 
 * // params, and result are optional user pointers, that we can ignore them here (our data is global)
 * static void myfunc(void* params, void* result, uint thread_id, uint job_id, uint worker_idx)
 * {  
 *     // for each worker, we have to go to start of it's data and calculate that region
 *     uint i = worker_idx*(100/dispatch_count);
 *     myarray_result[i] = myarray1[i]*myarray2[i];
 * }
 * 
 * tsk_initmgr(dispatch_count, 0, 0, 0);
 * // dispatch job (task)
 * uint job = tsk_dispatch(myfunc, TSK_CONTEXT_ALL, dispatch_count, NULL, NULL);
 * // wait for job to finish
 * tsk_wait(job);
 * // print result
 * puts("results:")
 * for (int i = 0; i < 100; i++)
 *     printf("%d\n", myarray_result[i]);
 * tsk_releasemgr();
 * @endcode
 * @ingroup taskman
 */

/**
 * Task context enumerator, in order to dispatch a task to threads, we can define it's running context 
 * @see tsk_dispatch
 * @ingroup taskman
 */
enum tsk_run_context
{
    TSK_CONTEXT_ALL, /**< Run in all threads, does not care if each thread is busy or not */
    TSK_CONTEXT_FREE, /**< Run in non-busy threads only */
    TSK_CONTEXT_ALL_NO_MAIN, /**< like TSK_CONTEXT_ALL but doesn't run task in the caller thread (main) */
    TSK_CONTEXT_FREE_NO_MAIN /* assign task to free threads, except the main one */
};

#define TSK_THREADS_ALL INT32_MAX

/**
 * Callback for task run, each callback is called within a thread, so it will give you the thread_id, 
 * running @e job_id, which is the Id that is created on @e tsk_dispath. And @e worker_idx which is a 
 * zero-based index of the running task. For example if you dispatch a task to 4 threads, there will be 
 * 4 calls in each thread with worker_idx(s) of 0, 1, 2 and 3.
 * @param params Custom user-defined params for task function, submitted by @e tsk_dispatch
 * @param result Result user-defined structure for task function, submitted by @e tsk_dispatch
 * @param thread_id Running thread ID
 * @param job_id Current running task ID
 * @param worker_idx Job index for task. For example, if task is submitted to 2 threads,
 * there would be (0, 1) indexes dispatched to each callback function
 * @see tsk_dispatch
 * @ingroup taskman
 */
typedef void (*pfn_tsk_run)(void* params, void* result, uint thread_id, uint job_id, 
    uint worker_idx);

/**
 * Initialize task manager, must call this function at the start of the program
 * @param thread_cnt Number of threads that task manager creates
 * @param localmem_perthread_sz local memory allocator (freelist) for each thread (in bytes). 
 * Local memory allocator can be fetched with @e tsk_get_localalloc function
 * @param tmpmem_perthread_sz Temp memory allocator (stack alloc) for each thread (in bytes). 
 * Temp memory allocator can be fetched with @e tsk_get_tmpalloc function
 * @param flags Not used (set to 0)
 * @ingroup taskman
 */
CORE_API result_t tsk_initmgr(uint thread_cnt, size_t localmem_perthread_sz,
                              size_t tmpmem_perthread_sz, uint flags);

/**
 * Release task manager and free task manager threads, must call this function at the end of the program
 * @ingroup taskman
 */
CORE_API void tsk_releasemgr();

/**
 * Dispatch a task (job) to multiple threads, task should be implemented by the user callback function.\n
 * @b Note that this function must be called from the main thread only, task manager does not support 
 * dispatches from differnt threads
 * @param run_fn Callback function for the task, function will run in each thread separately
 * @param ctx Defines how should the task be dispatched to threads
 * @param thread_cnt Maximum number of threads that the task will dispatch
 * @param params User defined pointer for input data for the callback
 * @param result User defined pointer for output data for the callback
 * @see pfn_tsk_run
 * @see tsk_wait
 * @see tsk_destroy
 * @ingroup taskman
 */
CORE_API uint tsk_dispatch(pfn_tsk_run run_fn, enum tsk_run_context ctx, uint thread_cnt, 
    void* params, void* result);

/** 
 * Run a task in user defined threads only, this function is for more advanced use when caller wants 
 * to dispatch a task to specific threads and knows what he is doing.
 * @param run_fn Callback function for the task, function will run in each thread separately
 * @param thread_idxs Array of zero-based index for threads to dispatch. For example if the task manager 
 * is initialized with 4 threads, an array of [0, 1, 2] dispatches the task to first 3 threads only.
 * @param thread_cnt Number of indexes in @e thread_idxs array
 * @param params User defined pointer for input data for the callback
 * @param result User defined pointer for output data for the callback
 * @see pfn_tsk_run
 * @ingroup taskman
 */
CORE_API uint tsk_dispatch_exclusive(pfn_tsk_run run_fn, const uint* thread_idxs, uint thread_cnt, 
    void* params, void* result);

/**
 * Destroys a task (job), user must call this function after he is done with dispatch 
 * @param job_id JobId of the dispatched task
 * @see tsk_dispatch
 * @see tsk_dispatch_exclusive
 * @ingroup taskman
 */
CORE_API void tsk_destroy(uint job_id);

/**
 * Blocks program execution until a specific task is done
 * @param job_id Job Id of the dispatched task
 * @see tsk_dispatch
 * @see tsk_dispatch_exclusive
 * @ingroup taskman
 */
CORE_API void tsk_wait(uint job_id);

/**
 * Checks if task is finished, does not block the program
 * @param job_id Job Id of the dispatched task
 * @return TRUE if task is finished
 * @see tsk_dispatch
 * @see tsk_dispatch_exclusive
 * @ingroup taskman
 */
CORE_API int tsk_check_finished(uint job_id);

/**
 * Returns allocator object for current running thread, local allocator memory is permanent, and 
 * it's contents won't reset after each task is finished
 * @ingroup taskman
 */
CORE_API struct allocator* tsk_get_localalloc(uint thread_id);

/**
 * Returns temp allocator for current running thread, temp allocator memory contents will be reset on 
 * the beginning of each task
 * @ingroup taskman
 */
CORE_API struct allocator* tsk_get_tmpalloc(uint thread_id);

/**
 * Get user defined @e params pointer for task Id
 * @ingroup taskman
 */
CORE_API void* tsk_get_params(uint job_id);

/**
 * Get user defined @e result pointer for task Id
 * @ingroup taskman
 */
CORE_API void* tsk_get_result(uint job_id);

#endif /* __TASKMGR_H__ */
