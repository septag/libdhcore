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

#ifndef __ZIP_H__
#define __ZIP_H__

/**
 * @defgroup zip Zip
 * Low-level buffer compression/decompression using miniz library (INFLATE)\n
 * @ingroup zip
 */

#include "types.h"
#include "core-api.h"
#include "file-io.h"

struct mz_zip_archive_tag;
typedef struct mz_zip_archive_tag* zip_t;

/**
 * @ingroup zip
 */
enum compress_mode
{
    COMPRESS_NORMAL = 0,
    COMPRESS_FAST,
    COMPRESS_BEST,
    COMPRESS_NONE
};

/**
 * Roughly estimate maximum size of the compressed buffer, it's recommended that you evaluate 
 * and allocated compressed buffer size with this function, then pass it to @e zip_compress
 * @param src_size Size (bytes) of uncompressed buffer
 * @return Estimated size of compressed target buffer
 * @see zip_compress
 * @ingroup zip
 */
CORE_API size_t zip_compressedsize(size_t src_size);

/**
 * Compress buffer in memory
 * @param dest_buffer Destination buffer that will be filled with compressed data
 * @param dest_size Maximum size of destiniation buffer, usually fetched by @e zip_compressedsize
 * @return actual Size of compressed buffer, returns 0 if 
 * @see zip_compressedsize
 * @ingroup zip
 */
CORE_API size_t zip_compress(void* dest_buffer, size_t dest_size, const void* buffer, size_t size, 
    enum compress_mode mode);

/**
 * Decompress buffer from memory
 * @param dest_buffer Uncompressed destination buffer 
 * @param dest_size Uncompressed buffer size, this value should be saved when buffer is compressed
 * @return actual Size of uncompressed buffer
 * @ingroup zip
 */
CORE_API size_t zip_decompress(void* dest_buffer, size_t dest_size, const void* buffer, size_t size);

CORE_API zip_t zip_open(const char *filepath);
CORE_API zip_t zip_open_mem(const char *buff, size_t buff_sz);

CORE_API void zip_close(zip_t zip);
CORE_API file_t zip_getfile(zip_t zip, const char *filepath, struct allocator *alloc);

#endif /* __ZIP_H__ */
