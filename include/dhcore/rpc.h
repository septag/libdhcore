/***********************************************************************************
 * Copyright (c) 2014, Sepehr Taghdisian
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

#ifndef __RPC_H__
#define __RPC_H__
 
/**
 * @defgroup rpc JSON-RPC
 * Adds JSON-RPC standard support. Application must provide it's own web server and handle responses.\n
 * This module eases the process of parsing JSON-RPC calls and translates them back-and-forth to program friendly 
 * data structures. Also checks for correct function signatures.\n
 * Example usage:
 * @code
 * // RPC callback for running Foo command: (A + B) -> C
 * static struct rpc_result* rpc_foo(struct rpc_vblock* results, struct rpc_vblock* params,
 *     int id, void* user_param)
 * {
 *     int A = rpc_vblock_geti(params, RPC_VALUE(A));
 *     int B = rpc_vblock_geti(params, RPC_VALUE(B));
 * 
 *     // make the result and return
 *     rpc_vblock_seti(results, RPC_VALUE(C), A+B);
 *     return rpc_make_result(results, id, NULL);
 * }
 * 
 * rpc_init();
 * 
 * // define input parameters
 * const struct rpc_value params[] = {
 *     {"A", RPC_VALUE_INT, RPC_OFFSET_AUTO, sizeof(int), 1, FALSE},
 *     {"B", RPC_VALUE_INT, RPC_OFFSET_AUTO, sizeof(int), 1, FALSE}
 * };
 * const struct rpc_value result[] = {
 *     {"C", RPC_VALUE_INT, RPC_OFFSET_AUTO, sizeof(int), 1, FALSE}
 * };
 * 
 * // register a JSON-RPC command called Foo 
 * rpc_registercmd("Foo", rpc_foo, params, 2, result, 1, "Adds two integers and returns result",
 *     NULL);
 * 
 * // webserver: upon receiving JSON-RPC string
 * const char* json_rpc = http_server_get_jsonrpc(); 
 * // process JSON-RPC
 * struct rpc_result* r = rpc_process(json_rpc);
 * http_server_respond(r->data.json.json);
 * 
 * rpc_release();
 * 
 * @endcode
 * @ingroup rpc
 */

#include "dhcore/types.h"
#include "dhcore/allocator.h"
#include "dhcore/hash-table.h"
#include "dhcore/vec-math.h"
#include "core-api.h"

/**
 * Macro to reference RPC parameter names, insert name without double quotes. Example: RPC_VALUE(myval)
 * @ingroup rpc
 */
#define RPC_VALUE(name) hash_str(#name)

 /**
  * This macro is used in registering RPC structure members, defines that member variable is packed 
  * and there is not need to put offset
  * @see rpc_registercmd
  * @ingroup rpc
  */
#define RPC_OFFSET_AUTO 0xFFFFFFFF

/**
 * Common RPC error codes 
 * @ingroup rpc
 */
enum rpc_error_code
{
    RPC_ERROR_METHODNOTFOUND = 1,
    RPC_ERROR_INVALIDARGS,
    RPC_ERROR_COMMANDFAIL,
    RPC_ERROR_AUTHFAIL,
    RPC_ERROR_DBFAIL,
    RPC_ERROR_ACCESSDENIED
};

/**
 * RPC value types used in registering structure members
 * @see rpc_registercmd
 * @ingroup rpc
 */
enum rpc_value_type
{
    RPC_VALUE_NULL = 0,
    RPC_VALUE_INT,
    RPC_VALUE_INT_ARRAY,
    RPC_VALUE_INT2,
    RPC_VALUE_INT3,
    RPC_VALUE_INT4,
    RPC_VALUE_FLOAT,
    RPC_VALUE_FLOAT2,
    RPC_VALUE_FLOAT3,
    RPC_VALUE_FLOAT4,
    RPC_VALUE_BOOL,
    RPC_VALUE_STRING,
    RPC_VALUE_STRING_ARRAY
};

/**
 * RPC value (structure member) descriptor, for registering RPC data structure, you should pass in 
 * arrays of @e rpc_value(s) for input and output params to define their structures
 * @see rpc_registercmd
 * @ingroup rpc
 */
struct rpc_value
{
    const char* name;   /**< Value name */
    enum rpc_value_type type; /**< Value type */
    uint offset;    /**< Offset (in bytes) inside the buffer, set RPC_OFFSET_AUTO for packed buffers */
    uint stride;    /**< Size of the single value in bytes, if it's an array, you should set it to the size of the single element */
    int array_cnt; /**< Number of elements if value is an array. Set 1 for values that are not arrays */
    int optional;   /**< Boolean value that defines if value can be optional */
};

/**
 * Value block, holds a collection of values.\n
 * Input and Output parameters of each RPC call, contains one of these blocks that you manipulate
 * @ingroup rpc
 */
struct rpc_vblock
{
    struct allocator* alloc;
    uint value_cnt;
    struct rpc_value* values;
    struct hashtable_fixed vtbl;   /* value table, key: name(hash), value: index to value */
    uint buff_size;
    uint8* buff;    /* buffer that holds all values */
};

/**
 * RPC error structure
 * @ingroup rpc
 */
struct rpc_error
{
    enum rpc_error_code code;   /**< Error codes */
    char desc[256]; /**< Error descriptions */
};

/**
 * RPC result type
 * @ingroup rpc
 */
enum rpc_result_type
{
    RPC_RESULT_JSONRPC, /**< Normal JSON rpc string */
    RPC_RESULT_BINARY /**< Binary blob */
};

/**
 * RPC result: Currently there are two types of result, JSON strings (which is in RPC standard)
 * , and binary result, which is essentially a binary blob of data 
 * @ingroup rpc
 */
struct rpc_result
{
    enum rpc_result_type type;  /**< Result type */

    union   {
        struct {
            char* json; /**< Null-terminated JSON result string */
            size_t json_sz; /**< String size */
        }   json;
        struct  {
            size_t bin_sz;  /**< Binary size in bytes */
            void* bin;  /**< Binary blob pointer */
        }   bin;
    }   data;
};

/**
 * Callback function for running RPC commands
 * @param results Resulting value block, user should fill this block and use @e rpc_make_result to 
 * return a valid JSON result
 * @param params Input parameters value block, user can fetch input values from this block
 * @param id Id of the JSON-RPC call coming from client
 * @param user_param User-defined pointer that is passed upon command registration
 * @see rpc_registercmd
 * @see rpc_make_result
 * @see rpc_make_result_bin
 * @see rpc_return_error
 * @ingroup rpc
 */
typedef struct rpc_result* (*pfn_rpc_cmd)(struct rpc_vblock* results, struct rpc_vblock* params, 
    int id, void* user_param);

/* vblock: collection of values */
CORE_API struct rpc_vblock* rpc_vblock_create(const struct rpc_value* values, uint value_cnt, 
    struct allocator* alloc);
CORE_API void rpc_vblock_destroy(struct rpc_vblock* vb);

CORE_API enum rpc_value_type rpc_vblock_gettype(struct rpc_vblock* vb, uint name_hash);

CORE_API float rpc_vblock_getf(struct rpc_vblock* vb, uint name_hash);
CORE_API int rpc_vblock_geti(struct rpc_vblock* vb, uint name_hash);
CORE_API int rpc_vblock_geti_idx(struct rpc_vblock* vb, uint name_hash, int idx);
CORE_API struct vec2i rpc_vblock_get2i(struct rpc_vblock* vb, uint name_hash);
CORE_API int rpc_vblock_getb(struct rpc_vblock* vb, uint name_hash);
CORE_API struct vec2f rpc_vblock_get2f(struct rpc_vblock* vb, uint name_hash);
CORE_API struct vec3f rpc_vblock_get3f(struct rpc_vblock* vb, uint name_hash);
CORE_API struct vec4f rpc_vblock_get4f(struct rpc_vblock* vb, uint name_hash);
CORE_API const char* rpc_vblock_gets(struct rpc_vblock* vb, uint name_hash);
CORE_API const char* rpc_vblock_gets_idx(struct rpc_vblock* vb, uint name_hash, int idx);
CORE_API int rpc_vblock_get_arrcnt(struct rpc_vblock* vb, uint name_hash);

CORE_API void rpc_vblock_setf(struct rpc_vblock* vb, uint name_hash, float val);
CORE_API void rpc_vblock_seti(struct rpc_vblock* vb, uint name_hash, int val);
CORE_API void rpc_vblock_seti_idx(struct rpc_vblock* vb, uint name_hash, int idx, int val);
CORE_API void rpc_vblock_set2i(struct rpc_vblock* vb, uint name_hash, const struct vec2i* val);
CORE_API void rpc_vblock_set2f(struct rpc_vblock* vb, uint name_hash, const struct vec2f* val);
CORE_API void rpc_vblock_set3f(struct rpc_vblock* vb, uint name_hash, const struct vec3f* val);
CORE_API void rpc_vblock_set4f(struct rpc_vblock* vb, uint name_hash, const struct vec4f* val);
CORE_API void rpc_vblock_setb(struct rpc_vblock* vb, uint name_hash, int val);
CORE_API void rpc_vblock_sets(struct rpc_vblock* vb, uint name_hash, const char* val);
CORE_API void rpc_vblock_sets_idx(struct rpc_vblock* vb, uint name_hash, int idx, const char* val);
CORE_API void rpc_vblock_set_arrcnt(struct rpc_vblock* vb, uint name_hash, int cnt);

/**
 * Initialize RPC manager, run before issueing any RPC calls
 * @ingroup rpc
 */
CORE_API result_t rpc_init();

/**
 * Release RPC manager
 * @ingroup rpc
 */
CORE_API void rpc_release();

/**
 * Makes RPC result from a parameter block, or make error result
 * @param ret Parameter block, likely the @e result parameter that is passed to command callback
 * @param id Id of the RPC command, this value is likely the @e id value that is passed to callback
 * @param err Error structure instead of a valid result, can pass NULL if no error occured
 * @ingroup rpc
 */
CORE_API struct rpc_result* rpc_make_result(struct rpc_vblock* ret, int id, struct rpc_error* err);

/**
 * Makes formatted RPC error result
 * @param id Id of the RPC command, this value is likely the @e id value that is passed to callback
 * @param code Error code
 * @param descfmt Formatted (printf style) description message
 * @ingroup rpc
 */
CORE_API struct rpc_result* rpc_return_error(int id, enum rpc_error_code code, const char* descfmt, ...);

/**
 * Makes a binary result, useful if you want to send binary or file blobs back to client
 * @param data Binary data pointer
 * @param data_sz Data size in bytes
 * @ingroup rpc
 */
CORE_API struct rpc_result* rpc_make_result_bin(const void* data, size_t data_sz);

/**
 * Process JSON-RPC string, run registered command callback, and make final result
 * @params json_rpc Valid JSON-RPC null-terminated string, normally fetched from webserver
 * @see rpc_result
 * @ingroup rpc
 */
CORE_API struct rpc_result* rpc_process(const char* json_rpc);

/**
 * After a successful call to @e rpc_process, user must call this function to free the result
 * @see rpc_process
 * @ingroup rpc
 */
CORE_API void rpc_freeresult(struct rpc_result* r);

/**
 * Register an RPC command
 * @param name Command name (method:[name] in JSON-RPC)
 * @param run_fn Callback function for command execution
 * @param params An array of @e rpc_values that defines input parameters for command
 * @param param_cnt Input parameters count (size of @e params array)
 * @param results An array of @e rpc_values that defines output parameters for command
 * @param result_cnt Output parameters count (size of @e results array)
 * @param desc Command description shown in help
 * @param user_param User defined pointer that will be passed to the callback
 * @see pfn_rpc_cmd
 * @ingroup rpc
 */
CORE_API result_t rpc_registercmd(const char* name, pfn_rpc_cmd run_fn, const struct rpc_value* params, 
    uint param_cnt, const struct rpc_value* results, uint result_cnt, const char* desc, 
    void* user_param);
   
#endif /* __RPC_H__ */