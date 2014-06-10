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

#include "types.h"
#include "allocator.h"
#include "hash-table.h"
#include "vec-math.h"
#include "core-api.h"

#define RPC_VALUE(name) hash_str(#name)
#define RPC_OFFSET_AUTO 0xFFFFFFFF
 
enum rpc_error_code
{
    RPC_ERROR_METHODNOTFOUND,
    RPC_ERROR_INVALIDARGS,
    RPC_ERROR_COMMANDFAIL,
    RPC_ERROR_AUTHFAIL,
    RPC_ERROR_DBFAIL,
    RPC_ERROR_ACCESSDENIED
};

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

struct rpc_value
{
    const char* name;
    enum rpc_value_type type; 
    uint offset;    /* offset inside parent buffer (bytes) */
    uint stride;    /* size of each element (bytes) */
    int array_cnt; /* number of array elements (def=1) */
    int optional;   /* TRUE is value is optional */
};

struct rpc_vblock
{
    struct allocator* alloc;
    uint value_cnt;
    struct rpc_value* values;
    struct hashtable_fixed vtbl;   /* value table, key: name(hash), value: index to value */
    uint buff_size;
    uint8* buff;    /* buffer that holds all values */
};

struct rpc_error
{
    enum rpc_error_code code;
    char desc[256];
};

struct rpc_result
{
    char* json;
    size_t json_sz;
};

/* callback for executing commands */
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

/* */
CORE_API result_t rpc_init();
CORE_API void rpc_release();

CORE_API struct rpc_result* rpc_make_result(struct rpc_vblock* ret, int id, struct rpc_error* err);
CORE_API struct rpc_result* rpc_return_error(int id, enum rpc_error_code code, const char* descfmt, ...);

CORE_API struct rpc_result* rpc_process(const char* json_rpc);
CORE_API void rpc_freeresult(struct rpc_result* r);

CORE_API result_t rpc_registercmd(const char* name, pfn_rpc_cmd run_fn, const struct rpc_value* params, 
    uint param_cnt, const struct rpc_value* results, uint result_cnt, const char* desc, 
    void* user_param);
   
#endif /* __RPC_H__ */