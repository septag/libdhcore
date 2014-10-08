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


 /**
 * @defgroup JSON JSON
 * Using cJSON library, this file is basically a wrapper to cjson\n
 * note: library is thread-safe as long as multiple threads do not manipulate same JSON tree
 * for more JSON information visit: http://www.JSON.org/fatfree.html\n
 * libcjson : http://sourceforge.net/projects/cjson/\n
 */

#ifndef __JSON_H__
#define __JSON_H__

#include "types.h"
#include "core-api.h"
#include "file-io.h"
#include "allocator.h"

/**
 * JSON node data types
 * @ingroup JSON
 */
enum json_type
{
    JSON_BOOL = 1, 
    JSON_NULL = 2,
    JSON_NUM = 3,
    JSON_STRING = 4,
    JSON_ARRAY = 5,
    JSON_OBJECT = 6
};

/**
 * basic JSON type, used in JSON functions, if =NULL then it is either uninitialized or error
 * @ingroup JSON
 */
struct cJSON;
typedef struct cJSON* json_t;

/**
 * Initialize JSON parser with custom (and fast) memory management
 * @ingroup JSON
 */
result_t json_init();

/**
 * Release JSON allocators and pre-allocated memory buffers
 * @see json_init
 * @ingroup JSON
 */
void json_release();

/**
 * Parse JSON file and create a root object, root object must be destroyed if not needed anymore
 * @param filepath is the full filepath of the JSON file on disk
 * @return JSON object, NULL if error occured
 * @ingroup JSON
 */
CORE_API json_t json_parsefile(const char* filepath);

/**
 * Parse JSON file and create a root object, root object must be destroyed if not needed anymore
 * @param f file handle of JSON file
 * @return JSON object, NULL if error occured
 * @ingroup JSON
 */
CORE_API json_t json_parsefilef(file_t f, struct allocator* tmp_alloc);

/**
 * Parse JSON string (buffer)
 * @param str JSON formatted string
 * @return JSON object, NULL if error occured
 * @ingroup JSON
 */
CORE_API json_t json_parsestring(const char* str);

/**
 * Save JSON data to file
 * @param filepath path to the file on the disk
 * @param trim trims output JSON data (no formatting) to be more optimized and compact
 * @ingroup JSON
 */
CORE_API result_t json_savetofile(json_t j, const char* filepath, int trim);

/**
 * Save JSON data to file handle
 * @param f file handle that is ready and opened for writing
 * @param trim trims output JSON data (no formatting) to be more optimized and compact
 * @ingroup JSON
 */
CORE_API result_t json_savetofilef(json_t j, file_t f, int trim);

/**
 * Save JSON data to buffer, returns resulting string. user should call @e json_deletebuffer after 
 * whatever the user does with the resulting JSON string
 * @param alloc allocator that is used to create the output buffer
 * @param outsize output buffer size, including /0 character
 * @param trim trims output JSON data (no formatting) to be more optimized and compact
 * @see json_deletebuffer
 * @ingroup JSON
 */
CORE_API char* json_savetobuffer(json_t j, size_t* outsize, int trim);

/**
 * Deletes previously created JSON buffer @see json_savetobuffer
 * @ingroup JSON
 */
CORE_API void json_deletebuffer(char* buffer);

/**
 * Destroys JSON object and free memory
 * @param j JSON object that is allocated previously
 * @ingroup JSON
 */
CORE_API void json_destroy(json_t j);

/* set/get functions for JSON items
 **
 * @ingroup JSON
 */
CORE_API void json_seti(json_t j, int n);

/**
 * @ingroup JSON
 */
CORE_API void json_setf(json_t j, float f);

/**
 * @ingroup JSON
 */
CORE_API void json_sets(json_t j, const char* str);

/**
 * @ingroup JSON
 */
CORE_API void json_setb(json_t j, int b);

/**
 * @ingroup JSON
 */
CORE_API int json_geti(json_t j);

/**
 * @ingroup JSON
 */
CORE_API float json_getf(json_t j);

/**
 * @ingroup JSON
 */
CORE_API const char* json_gets(json_t j);

/**
 * @ingroup JSON
 */
CORE_API int json_getb(json_t j);

/**
 * Gets data type of the provided JSON node
 * @see json_type
 * @ingroup JSON
 */
CORE_API enum json_type json_gettype(json_t j);

/**
 * @ingroup JSON
 */
CORE_API int json_geti_child(json_t parent, const char* name, int def_value);

/**
 * @ingroup JSON
 */
CORE_API float json_getf_child(json_t parent, const char* name, float def_value);

/**
 * @ingroup JSON
 */
CORE_API const char* json_gets_child(json_t parent, const char* name, const char* def_value);

/**
 * @ingroup JSON
 */
CORE_API int json_getb_child(json_t parent, const char* name, int def_value);

/**
 * get array size (number of items) from a JSON array item
 * @ingroup JSON
 */
CORE_API int json_getarr_count(json_t j);

/**
 * get array item from a JSON array item
 * @param idx zero-based index to the array
 * @ingroup JSON
 */
CORE_API json_t json_getarr_item(json_t j, int idx);

/**
 * get child item from an JSON object referenced by a name
 * @ingroup JSON
 */
CORE_API json_t json_getitem(json_t j, const char* name);


/* creating JSON items for different types
 **
 * @ingroup JSON
 */
CORE_API json_t json_create_null();

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_obj();

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_bool(int b);

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_num(fl64 n);

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_str(const char* str);

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_arr();

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_arri(const int* nums, int count);

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_arrf(const float* nums, int count);

/**
 * @ingroup JSON
 */
CORE_API json_t json_create_arrs(const char** strs, int count);

/**
 * add single item to array type
 * @ingroup JSON
 */
CORE_API void json_additem_toarr(json_t arr, json_t item);

/**
 * add item to object type
 * @ingroup JSON
 */
CORE_API void json_additem_toobj(json_t obj, const char* name, json_t item);

/**
 * @ingroup JSON
 */
CORE_API void json_additem_toobj_nodup(json_t obj, const char* name, json_t item);

/**
 * @ingroup JSON
 */
CORE_API void json_replaceitem_inobj(json_t obj, const char* name, json_t item);

/**
 * @ingroup JSON
 */
CORE_API void json_replaceitem_inarr(json_t obj, int idx, json_t item);

#ifdef __cplusplus
namespace dh {

class JNode
{
private:
    json_t m_j = nullptr;

public:
    JNode(json_t j)
    {
        m_j = j;
    }

    JNode(double n)
    {
        m_j = json_create_num(n);
    }

    JNode(float n)
    {
        m_j = json_create_num(n);
    }

    JNode(int n)
    {
        m_j = json_create_num(n);
    }

    JNode(uint n)
    {
        m_j = json_create_num(n);
    }

    JNode(const char *str)
    {
        if (str != nullptr)
            m_j = json_create_str(str);
        else
            m_j = json_create_null();
    }

    JNode(bool b)
    {
        m_j = json_create_bool(b);
    }

    JNode(const int *nums, int count)
    {
        m_j = json_create_arri(nums, count);
    }

    JNode(const float *nums, int count)
    {
        m_j = json_create_arrf(nums, count);
    }

    JNode(const char **strs, int count)
    {
        m_j = json_create_arrs(strs, count);
    }

    JNode& add_obj(const char *name, const JNode &jnode)
    {
        json_additem_toobj(m_j, name, jnode.m_j);
        return *this;
    }

    JNode& add_obj_unique(const char *name, const JNode &jnode)
    {
        json_additem_toobj_nodup(m_j, name, jnode.m_j);
        return *this;
    }

    JNode& add_array_item(const JNode &jnode)
    {
        json_additem_toarr(m_j, jnode.m_j);
        return *this;
    }

    JNode& replace_obj(const char *name, const JNode &jnode)
    {
        json_replaceitem_inobj(m_j, name, jnode.m_j);
        return *this;
    }

    JNode& replace_array_item(int idx, const JNode &jnode)
    {
        json_replaceitem_inarr(m_j, idx, jnode.m_j);
        return *this;
    }

    static JNode create_obj()
    {
        return JNode(json_create_obj());
    }

    static JNode create_array()
    {
        return JNode(json_create_arr());
    }

    void destroy()
    {
        if (m_j)
            json_destroy(m_j);
        m_j = nullptr;
    }

    const char* to_str() const  {   return json_gets(m_j);  }
    int to_int() const          {   return json_geti(m_j);  }
    float to_float() const      {   return json_getf(m_j);  }
    bool to_bool() const        {   return (bool)json_getb(m_j);    }

    const char* child_str(const char *name, const char *def_val = "") const
    {
        return json_gets_child(m_j, name, def_val);
    }

    int child_int(const char *name, int def_val = 0) const
    {
        return json_geti_child(m_j, name, def_val);
    }

    float child_float(const char *name, float def_val = 0.0f) const
    {
        return json_getf_child(m_j, name, def_val);
    }

    bool child_bool(const char *name, bool def_val = false) const
    {
        return (bool)json_getb_child(m_j, name, def_val);
    }

    enum json_type type() const     {   return json_gettype(m_j);   }

    operator json_t() { return m_j; }
    operator const json_t() const { return m_j; }
};

}
#endif


#endif /* __JSON_H__ */
