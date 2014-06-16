/***********************************************************************************
 * Copyright (c) 2014, Tochal Co. (wwww.tochalco.com)
 * All rights reserved.
 *
 * Author: Sepehr Taghdisian
 *
 ***********************************************************************************/

#include <stdarg.h>

#include "dhcore/core.h"
#include "dhcore/stack-alloc.h"
#include "dhcore/array.h"
#include "dhcore/json.h"

#include "rpc.h"

#define MAX_COMMAND_LIST    128

/* types */
struct rpc_cmd
{
    char name[32];
    struct rpc_value* results;
    uint result_cnt;
    struct rpc_value* params;
    uint param_cnt;
    void* user_param;
    pfn_rpc_cmd run_fn;
    char desc[256];
};

struct rpc_mgr
{
    struct array cmds;  /* item: rpc_cmd */
    struct hashtable_open cmd_tbl;  /* key: name, value: cmd_id */
};

/* globals */
static struct rpc_mgr* g_rpc = NULL;

/*************************************************************************************************/
INLINE struct rpc_cmd* rpc_cmd_get(uint id)
{
    return &ARR_ITEM(g_rpc->cmds, struct rpc_cmd, id-1);
}

uint rpc_cmd_find(const char* name)
{
    struct hashtable_item* item = hashtable_open_find(&g_rpc->cmd_tbl, hash_str(name));
    if (item != NULL)
        return (uint)item->value;
    return 0;
}

static const char* rpc_get_valuetype_str(enum rpc_value_type type, uint stride)
{
    static const char* types[] = {
        "NULL",
        "INT",
        "INT_ARRAY",
        "INT2",
        "INT3",
        "INT4",
        "FLOAT",
        "FLOAT2",
        "FLOAT3",
        "FLOAT4",
        "BOOL",
        "STRING",
        "STRING_ARRAY"
    };
    static char str[32];

    if (type == RPC_VALUE_STRING)   {
        /* add string size too */
        snprintf(str, sizeof(str), "STRING[%d]", stride-1);
        return str;
    }

    return types[(int)type];
}

/* internal methods */
static struct rpc_result* rpc_method_help(struct rpc_vblock* results, struct rpc_vblock* params, 
    int id, void* user_param)
{
    const char* name = rpc_vblock_gets(params, RPC_VALUE(Name));
    uint cmd_id = rpc_cmd_find(name);
    if (cmd_id == 0)    {
        return rpc_return_error(id, RPC_ERROR_COMMANDFAIL, "method '%s' not found", name);
    }   else    {
        struct rpc_cmd* cmd = rpc_cmd_get(cmd_id);

        rpc_vblock_sets(results, RPC_VALUE(Method), name);
        rpc_vblock_sets(results, RPC_VALUE(Description), cmd->desc);

        /* params string */
        char param_str[512];
        char param_line[128];
        param_str[0] = 0;
        for (uint i = 0; i < cmd->param_cnt; i++)    {
            struct rpc_value* value = &cmd->params[i];
            char arr_str[32];
            arr_str[0] = 0;
            char optional[32];
            optional[0] = 0;
            if (value->array_cnt > 1)
                snprintf(arr_str, sizeof(arr_str), " (Array Max = %d)", value->array_cnt);
            if (value->optional)
                strcpy(optional, " - [OPTIONAL]");
            snprintf(param_line, sizeof(param_line), "%s: %s%s%s\n", value->name,
                rpc_get_valuetype_str(value->type, value->stride), arr_str, optional);
            strcat(param_str, param_line);
        }
        rpc_vblock_sets(results, RPC_VALUE(Params), param_str);

        /* results string */
        char result_str[512];
        char result_line[128];
        result_str[0] = 0;
        for (uint i = 0; i < cmd->result_cnt; i++)    {
            struct rpc_value* value = &cmd->results[i];
            char arr_str[32];
            arr_str[0] = 0;
            if (value->array_cnt > 1)
                snprintf(arr_str, sizeof(arr_str), " (Array Max = %d)", value->array_cnt);
            snprintf(result_line, sizeof(result_line), "%s: %s%s\n", value->name,
                rpc_get_valuetype_str(value->type, value->stride), arr_str);
            strcat(result_str, result_line);
        }
        rpc_vblock_sets(results, RPC_VALUE(Result), result_str);

        return rpc_make_result(results, id, NULL);
    }
}

static struct rpc_result* rpc_method_listmethods(struct rpc_vblock* results, struct rpc_vblock* params, 
    int id, void* user_param)
{
    uint i = 0;
    for (i = 0; i < g_rpc->cmds.item_cnt && i < MAX_COMMAND_LIST; i++)     {
        struct rpc_cmd* cmd = &ARR_ITEM(g_rpc->cmds, struct rpc_cmd, i);
        rpc_vblock_sets_idx(results, RPC_VALUE(Methods), i, cmd->name);
    }
    rpc_vblock_set_arrcnt(results, RPC_VALUE(Methods), i); 

    return rpc_make_result(results, id, NULL);
}

/* */
struct rpc_vblock* rpc_vblock_create(const struct rpc_value* values, uint value_cnt, 
    struct allocator* alloc)
{
    /* estimate size */
    size_t buff_sz = 0;
    for (uint i = 0; i < value_cnt; i++)        
        buff_sz += values[i].stride*values[i].array_cnt;

    size_t total_sz = 
        sizeof(struct rpc_vblock) +
        sizeof(struct rpc_value)*value_cnt + 
        hashtable_fixed_estimate_size(value_cnt) +
        buff_sz;

    /* create the whole memory block */
    struct stack_alloc stack_mem;
    struct allocator stack_alloc;
    result_t r;

    r = mem_stack_create(alloc, &stack_mem, total_sz, 0);
    if (IS_FAIL(r)) 
        return NULL;
    mem_stack_bindalloc(&stack_mem, &stack_alloc);

    struct rpc_vblock* vb = (struct rpc_vblock*)A_ALLOC(&stack_alloc, sizeof(struct rpc_vblock), 
        0);
    vb->alloc = alloc;
    vb->values = (struct rpc_value*)A_ALLOC(&stack_alloc, sizeof(struct rpc_value)*value_cnt, 
        0);
    memcpy(vb->values, values, sizeof(struct rpc_value)*value_cnt);
    vb->value_cnt = value_cnt;

    hashtable_fixed_create(&stack_alloc, &vb->vtbl, value_cnt, 0);
    for (uint i = 0; i < value_cnt; i++)
        hashtable_fixed_add(&vb->vtbl, hash_str(values[i].name), i);

    vb->buff = (uint8*)A_ALLOC(&stack_alloc, buff_sz, 0);
    memset(vb->buff, 0x00, buff_sz);

    return vb;
}

void rpc_vblock_destroy(struct rpc_vblock* vb)
{
    A_ALIGNED_FREE(vb->alloc, vb);
}

static struct rpc_value* rpc_lookup_value(struct rpc_vblock* vb, uint name_hash)
{
    struct hashtable_item* item = hashtable_fixed_find(&vb->vtbl, name_hash);
    if (item != NULL)
        return &vb->values[item->value];
    return NULL;
}

enum rpc_value_type rpc_vblock_gettype(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)
        return value->type;
    else
        return RPC_VALUE_NULL;
}

float rpc_vblock_getf(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_FLOAT)  {
        return *((float*)(vb->buff + value->offset));
    }
    return 0.0f;
}

int rpc_vblock_geti(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_INT)  {
        return *((int*)(vb->buff + value->offset));
    }    
    return 0;
}

int rpc_vblock_geti_idx(struct rpc_vblock* vb, uint name_hash, int idx)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_INT_ARRAY)  {
        ASSERT(idx < value->array_cnt);
        return *((int*)(vb->buff + value->offset + value->stride*idx));
    }    
    return 0;
}

int rpc_vblock_get_arrcnt(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)
        return value->array_cnt;
    return 0;       
}

struct vec2i rpc_vblock_get2i(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_INT2)  {
        return *((struct vec2i*)(vb->buff + value->offset));
    }    
    struct vec2i v;
    vec2i_seti(&v, 0, 0);
    return v;
}

int rpc_vblock_getb(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_BOOL)  {
        return *((int*)(vb->buff + value->offset));
    }    
    return FALSE;
}

struct vec2f rpc_vblock_get2f(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_FLOAT2)  {
        return *((struct vec2f*)(vb->buff + value->offset));
    }    
    struct vec2f v;
    vec2f_setf(&v, 0.0f, 0.0f);
    return v;
}

struct vec3f rpc_vblock_get3f(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_FLOAT3)  {
        return *((struct vec3f*)(vb->buff + value->offset));
    }    
    struct vec3f v;
    vec3_setf(&v, 0.0f, 0.0f, 0.0f);
    return v;
}

struct vec4f rpc_vblock_get4f(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_FLOAT4)  {
        return *((struct vec4f*)(vb->buff + value->offset));
    }    
    struct vec4f v;
    vec4_setf(&v, 0.0f, 0.0f, 0.0f, 0.0f);
    return v;
}

const char* rpc_vblock_gets(struct rpc_vblock* vb, uint name_hash)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_STRING)  {
        return (const char*)(vb->buff + value->offset);
    }    
    return "";
}

const char* rpc_vblock_gets_idx(struct rpc_vblock* vb, uint name_hash, int idx)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL && value->type == RPC_VALUE_STRING_ARRAY)  {
        ASSERT(idx < value->array_cnt);
        return (const char*)(vb->buff + value->offset + value->stride*idx);
    }    
    return "";
}

void rpc_vblock_setf(struct rpc_vblock* vb, uint name_hash, float val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_FLOAT);
        *((float*)(vb->buff + value->offset)) = val;
    }
}

void rpc_vblock_seti(struct rpc_vblock* vb, uint name_hash, int val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_INT);
        *((int*)(vb->buff + value->offset)) = val;
    }
}

void rpc_vblock_seti_idx(struct rpc_vblock* vb, uint name_hash, int idx, int val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_INT_ARRAY);
        ASSERT(idx < value->array_cnt);
        *((int*)(vb->buff + value->offset + idx*value->stride)) = val;
    }
}

void rpc_vblock_set2i(struct rpc_vblock* vb, uint name_hash, const struct vec2i* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_INT2);
        vec2i_setv((struct vec2i*)(vb->buff + value->offset), val);
    }
}

void rpc_vblock_set2f(struct rpc_vblock* vb, uint name_hash, const struct vec2f* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_FLOAT2);
        vec2f_setv((struct vec2f*)(vb->buff + value->offset), val);
    }
}

void rpc_vblock_set3f(struct rpc_vblock* vb, uint name_hash, const struct vec3f* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_FLOAT3);
        vec3_setv((struct vec3f*)(vb->buff + value->offset), val);
    }
}

void rpc_vblock_set4f(struct rpc_vblock* vb, uint name_hash, const struct vec4f* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_FLOAT4);
        vec4_setv((struct vec4f*)(vb->buff + value->offset), val);
    }
}

void rpc_vblock_setb(struct rpc_vblock* vb, uint name_hash, int val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_BOOL);
        *((int*)(vb->buff + value->offset)) = val;
    }
}

void rpc_vblock_sets(struct rpc_vblock* vb, uint name_hash, const char* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_STRING);
        memcpy(vb->buff + value->offset, val, minui(strlen(val)+1, value->stride));
    }
}

void rpc_vblock_sets_idx(struct rpc_vblock* vb, uint name_hash, int idx, const char* val)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->type == RPC_VALUE_STRING_ARRAY);
        ASSERT(idx < value->array_cnt);
        memcpy(vb->buff + value->offset + idx*value->stride, val, 
            minui(strlen(val)+1, value->stride));
    }
}

void rpc_vblock_set_arrcnt(struct rpc_vblock* vb, uint name_hash, int cnt)
{
    struct rpc_value* value = rpc_lookup_value(vb, name_hash);
    if (value != NULL)  {
        ASSERT(value->array_cnt >= cnt);
        value->array_cnt = cnt;
    }
}

result_t rpc_init()
{
    if (g_rpc != NULL) 
        return RET_FAIL;

    log_print(LOG_TEXT, "init json-rpc ...");

    result_t r;
    g_rpc = (struct rpc_mgr*)ALLOC(sizeof(struct rpc_mgr), 0);
    if (g_rpc == NULL)
        return err_printn(__FILE__, __LINE__, RET_OUTOFMEMORY);
    memset(g_rpc, 0x00, sizeof(struct rpc_mgr));

    r = arr_create(mem_heap(), &g_rpc->cmds, sizeof(struct rpc_cmd), 20, 40, 0);
    if (IS_FAIL(r)) 
        return err_printn(__FILE__, __LINE__, RET_OUTOFMEMORY);

    r = hashtable_open_create(mem_heap(), &g_rpc->cmd_tbl, 20, 40, 0);
    if (IS_FAIL(r))
        return err_printn(__FILE__, __LINE__, RET_OUTOFMEMORY);

    /* register help method */
    const struct rpc_value help_params[] = {
        {"Name", RPC_VALUE_STRING, 0, 32, 1, FALSE}
    };
    const struct rpc_value help_res[] = {
        {"Method", RPC_VALUE_STRING, 0, 32, 1, FALSE},
        {"Description", RPC_VALUE_STRING, RPC_OFFSET_AUTO, 256, 1, FALSE},
        {"Params", RPC_VALUE_STRING, RPC_OFFSET_AUTO, 512, 1, FALSE},
        {"Result", RPC_VALUE_STRING, RPC_OFFSET_AUTO, 512, 1, FALSE}
    };

    rpc_registercmd("Help", rpc_method_help, help_params,
        sizeof(help_params)/sizeof(struct rpc_value), help_res,
        sizeof(help_res)/sizeof(struct rpc_value),
        "show help info for specific method", NULL);

    const struct rpc_value list_result[] = {
        {"Methods", RPC_VALUE_STRING_ARRAY, 0, 32, MAX_COMMAND_LIST, FALSE}
    };
    rpc_registercmd("ListMethods", rpc_method_listmethods, NULL, 0, list_result, 1, 
        "", NULL);

    return RET_OK;
}

void rpc_release()
{
    if (g_rpc != NULL)  {
        hashtable_open_destroy(&g_rpc->cmd_tbl);

        for (uint i = 0; i < g_rpc->cmds.item_cnt; i++) {
            struct rpc_cmd* c = rpc_cmd_get(i+1);
            if (c->params != NULL)
                FREE(c->params);
            if (c->results != NULL)
                FREE(c->results);
        }
        arr_destroy(&g_rpc->cmds);

        FREE(g_rpc);
        g_rpc = NULL;

        log_print(LOG_TEXT, "json-rpc released.");
    }
}

struct rpc_result* rpc_return_error(int id, enum rpc_error_code code, const char* descfmt, ...)
{
    struct rpc_error err;

    err.desc[0] = 0;
    err.code = code;

    va_list args;
    va_start(args, descfmt);
    vsnprintf(err.desc, sizeof(err.desc), descfmt, args);
    va_end(args);

    /* */
    log_printf(LOG_ERROR, "%s (code:%d)", err.desc, (int)code);

    return rpc_make_result(NULL, id, &err);
}

struct rpc_result* rpc_process(const char* json_rpc)
{
    /* parse json */
    json_t jroot = json_parsestring(json_rpc);
    if (jroot == NULL)  {
        err_printf(__FILE__, __LINE__, "JSON-RPC: parsing json '%s' failed", json_rpc);
        return NULL;
    }
    
    /* get method and ID */
    const char* method = json_gets_child(jroot, "method", "");
    int id = json_geti_child(jroot, "id", -1);
    json_t jparams = json_getitem(jroot, "params");

    uint cmd_id = rpc_cmd_find(method);
    if (cmd_id == 0)    {
        json_destroy(jroot);
        return rpc_return_error(id, RPC_ERROR_METHODNOTFOUND, "method '%s' not found", method);
    }
    struct rpc_cmd* cmd = rpc_cmd_get(cmd_id);

    /* create and parse params */
    struct rpc_vblock* vbparams = NULL;

    vbparams = rpc_vblock_create(cmd->params, cmd->param_cnt, mem_heap());
    ASSERT(vbparams);
    if (jparams != NULL)    {
        for (int i = 0; i < cmd->param_cnt; i++)    {
            struct rpc_value* p = &cmd->params[i];

            json_t jp = json_getitem(jparams, p->name);
            if (jp != NULL) {
                enum rpc_value_type type = rpc_vblock_gettype(vbparams, hash_str(p->name));
                if (type == RPC_VALUE_NULL) {
                    json_destroy(jroot);
                    rpc_vblock_destroy(vbparams);
                    return rpc_return_error(id, RPC_ERROR_INVALIDARGS, "parameter '%s' doesn't exist"
                        " in method signature", p->name);
                }
                switch (type)   {
                    case RPC_VALUE_INT:
                    rpc_vblock_seti(vbparams, hash_str(p->name), json_geti(jp));
                    break;
                    case RPC_VALUE_INT2:
                    {
                        struct vec2i v;
                        for (int i = 0, c = mini(json_getarr_count(jp), 2); i < c; i++)
                            v.n[i] = json_geti(json_getarr_item(jp, i));
                        rpc_vblock_set2i(vbparams, hash_str(p->name), &v);
                    }
                    break;
                    case RPC_VALUE_INT3:
                    case RPC_VALUE_INT4:
                    ASSERT(0);
                    break;
                    case RPC_VALUE_INT_ARRAY:
                    {
                        int max_cnt = p->array_cnt;
                        uint name_hash = hash_str(p->name);
                        int c = mini(json_getarr_count(jp), max_cnt);
                        for (int i = 0; i < c; i++)   {
                            rpc_vblock_seti_idx(vbparams, name_hash, i,
                                json_geti(json_getarr_item(jp, i)));
                        }
                        rpc_vblock_set_arrcnt(vbparams, name_hash, c);
                    }
                    break;
                    case RPC_VALUE_FLOAT:
                    rpc_vblock_setf(vbparams, hash_str(p->name), json_getf(jp));
                    break;
                    case RPC_VALUE_FLOAT2:
                    {
                        struct vec2f v;
                        for (int i = 0, c = mini(json_getarr_count(jp), 2); i < c; i++)
                            v.f[i] = json_getf(json_getarr_item(jp, i));
                        rpc_vblock_set2f(vbparams, hash_str(p->name), &v);
                    }
                    break;
                    case RPC_VALUE_FLOAT3:
                    {
                        struct vec3f v;
                        for (int i = 0, c = mini(json_getarr_count(jp), 3); i < c; i++)
                            v.f[i] = json_getf(json_getarr_item(jp, i));
                        rpc_vblock_set3f(vbparams, hash_str(p->name), &v);
                    }
                    break;
                    case RPC_VALUE_FLOAT4:
                    {
                        struct vec4f v;
                        for (int i = 0, c = mini(json_getarr_count(jp), 4); i < c; i++)
                            v.f[i] = json_getf(json_getarr_item(jp, i));
                        rpc_vblock_set3f(vbparams, hash_str(p->name), &v);
                    }
                    break;
                    case RPC_VALUE_BOOL:
                    rpc_vblock_setb(vbparams, hash_str(p->name), json_geti(jp));
                    break;
                    case RPC_VALUE_STRING:
                    rpc_vblock_sets(vbparams, hash_str(p->name), json_gets(jp));
                    break;

                    case RPC_VALUE_STRING_ARRAY:
                    {
                        int max_cnt = p->array_cnt;
                        uint name_hash = hash_str(p->name);
                        int c = mini(json_getarr_count(jp), max_cnt);
                        for (int i = 0; i < c; i++)   {
                            rpc_vblock_sets_idx(vbparams, name_hash, i, 
                                json_gets(json_getarr_item(jp, i)));
                        }
                        rpc_vblock_set_arrcnt(vbparams, name_hash, c);
                    }
                    break;                    
                    default:
                    break;
                }
            }   else if (!p->optional)  {
                json_destroy(jroot);
                rpc_vblock_destroy(vbparams);
                return rpc_return_error(id, RPC_ERROR_INVALIDARGS, "missing paramter '%s'", p->name);
            }
        }
    }   /*endif: jparams != NULL */

    /* run method */
    struct rpc_vblock* vbres = rpc_vblock_create(cmd->results, cmd->result_cnt, mem_heap());
    ASSERT(vbres);
    struct rpc_result* r = cmd->run_fn(vbres, vbparams, id, cmd->user_param);
    rpc_vblock_destroy(vbres);
    rpc_vblock_destroy(vbparams);

    return r;
}

struct rpc_result* rpc_make_result(struct rpc_vblock* ret, int id, struct rpc_error* err)
{
    json_t jroot = json_create_obj();    
    json_additem_toobj(jroot, "id", json_create_num(id));
    if (ret != NULL)    {
        json_t jresult = json_create_obj(jroot);

        for (uint i = 0; i < ret->value_cnt; i++)   {
            struct rpc_value* value = &ret->values[i];
            switch (value->type)    {
                case RPC_VALUE_INT:
                json_additem_toobj(jresult, value->name, 
                    json_create_num(rpc_vblock_geti(ret, hash_str(value->name))));
                break;
                case RPC_VALUE_INT_ARRAY:
                {
                    uint name_hash = hash_str(value->name);
                    json_t jints = json_create_arr();
                    for (int k = 0; k< value->array_cnt; k++)    {
                        json_additem_toarr(jints, 
                            json_create_num(rpc_vblock_geti_idx(ret, name_hash, k)));
                    }
                    json_additem_toobj(jresult, value->name, jints);
                }
                break;
                case RPC_VALUE_INT2:
                json_additem_toobj(jresult, value->name, 
                    json_create_arri(rpc_vblock_get2i(ret, hash_str(value->name)).n, 2));
                break;
                case RPC_VALUE_INT3:
                case RPC_VALUE_INT4:
                ASSERT(0);
                case RPC_VALUE_FLOAT:
                json_additem_toobj(jresult, value->name, 
                    json_create_num(rpc_vblock_geti(ret, hash_str(value->name))));
                break;
                case RPC_VALUE_FLOAT2:
                json_additem_toobj(jresult, value->name, 
                    json_create_arrf(rpc_vblock_get2f(ret, hash_str(value->name)).f, 2));
                break;
                case RPC_VALUE_FLOAT3:
                json_additem_toobj(jresult, value->name, 
                    json_create_arrf(rpc_vblock_get3f(ret, hash_str(value->name)).f, 3));
                break;
                case RPC_VALUE_FLOAT4:
                json_additem_toobj(jresult, value->name, 
                    json_create_arrf(rpc_vblock_get4f(ret, hash_str(value->name)).f, 4));
                break;
                case RPC_VALUE_BOOL:
                json_additem_toobj(jresult, value->name, 
                    json_create_bool(rpc_vblock_getb(ret, hash_str(value->name))));
                break;
                case RPC_VALUE_STRING:
                json_additem_toobj(jresult, value->name, 
                    json_create_str(rpc_vblock_gets(ret, hash_str(value->name))));
                break;
                case RPC_VALUE_STRING_ARRAY:
                {
                    uint name_hash = hash_str(value->name);
                    json_t jstrs = json_create_arr();
                    for (int k = 0; k< value->array_cnt; k++)    {
                        json_additem_toarr(jstrs, 
                            json_create_str(rpc_vblock_gets_idx(ret, name_hash, k)));
                    }
                    json_additem_toobj(jresult, value->name, jstrs);
                }
                break;
                default:
                ASSERT(0);
            }
        }
        json_additem_toobj(jroot, "result", jresult);
        json_additem_toobj(jroot, "error", json_create_null());
    }   else if (err != NULL)   {
        json_additem_toobj(jroot, "result", json_create_null());
        json_t jerr = json_create_obj();
        json_additem_toobj(jerr, "code", json_create_num((fl64)err->code));
        json_additem_toobj(jerr, "description", json_create_str(err->desc));
        json_additem_toobj(jroot, "error", jerr);
    }   else    {
        json_destroy(jroot);
        ASSERT(0);
    }
    struct rpc_result* r = (struct rpc_result*)ALLOC(sizeof(struct rpc_result), 0);

    if (r != NULL)  {
        r->type = RPC_RESULT_JSONRPC;
        r->data.json.json_sz = 0;
#if _DEBUG_
        int trim = FALSE;
#else
        int trim = TRUE;
#endif
        r->data.json.json = json_savetobuffer(jroot, &r->data.json.json_sz, trim);
    }
    json_destroy(jroot);

    return r;
}

struct rpc_result* rpc_make_result_bin(const void* data, size_t data_sz)
{
    if (data_sz == 0)
        return NULL;

    struct rpc_result* r = (struct rpc_result*)ALLOC(sizeof(struct rpc_result), 0);

    if (r != NULL)  {
        r->type = RPC_RESULT_BINARY;

        r->data.bin.bin_sz = data_sz;
        r->data.bin.bin = ALLOC(data_sz, 0);
        memcpy(r->data.bin.bin, data, data_sz);
    }

    return r;
}

void rpc_freeresult(struct rpc_result* r)
{
    switch (r->type)    {
        case RPC_RESULT_JSONRPC:
        if (r->data.json.json != NULL)
            json_deletebuffer(r->data.json.json);
        break;
        case RPC_RESULT_BINARY:
        if (r->data.bin.bin != NULL)
            FREE(r->data.bin.bin);
        break;
    }
    FREE(r);
}

result_t rpc_registercmd(const char* name, pfn_rpc_cmd run_fn, const struct rpc_value* params, 
    uint param_cnt, const struct rpc_value* results, uint result_cnt, const char* desc, 
    void* user_param)
{
    ASSERT(run_fn);
    ASSERT(name);

    /* estimate size */
    struct rpc_cmd* cmd = (struct rpc_cmd*)arr_add(&g_rpc->cmds);
    ASSERT(cmd);
    memset(cmd, 0x00, sizeof(struct rpc_cmd));

    str_safecpy(cmd->name, sizeof(cmd->name), name);
    cmd->user_param = user_param;
    cmd->run_fn = run_fn;
    uint id = g_rpc->cmds.item_cnt;

    if (param_cnt > 0)  {
        cmd->params = (struct rpc_value*)ALLOC(sizeof(struct rpc_value)*param_cnt, 0);
        ASSERT(cmd->params);
        memcpy(cmd->params, params, param_cnt*sizeof(struct rpc_value));
        cmd->param_cnt = param_cnt;
    }

    if (result_cnt > 0) {
        cmd->results = (struct rpc_value*)ALLOC(sizeof(struct rpc_value)*result_cnt, 0);
        ASSERT(cmd->results);
        memcpy(cmd->results, results, result_cnt*sizeof(struct rpc_value));
        cmd->result_cnt = result_cnt;
    }

    /* fix param/result offsets */
    for (uint i = 0; i < param_cnt; i++)    {
        if (cmd->params[i].offset == RPC_OFFSET_AUTO)   {
            cmd->params[i].offset = (i != 0) ? 
                cmd->params[i-1].offset + cmd->params[i-1].stride*cmd->params[i-1].array_cnt :
                0;
        }
    }

    for (uint i = 0; i < result_cnt; i++)    {
        if (cmd->results[i].offset == RPC_OFFSET_AUTO)   {
            cmd->results[i].offset = (i != 0) ? 
                cmd->results[i-1].offset + cmd->results[i-1].stride*cmd->results[i-1].array_cnt :
                0;
        }
    }

    str_safecpy(cmd->desc, sizeof(cmd->desc), desc);

    /* add to hash-table */
    return hashtable_open_add(&g_rpc->cmd_tbl, hash_str(name), id);    
}

