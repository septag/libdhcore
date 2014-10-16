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


#ifndef __FILEIO_H__
#define __FILEIO_H__

#include "types.h"
#include "core-api.h"
#include "allocator.h"
#include "mem-mgr.h"

/**
 * @defgroup fileio File manager
 */

/* fwd declarations */
struct pak_file;

/**
 * Basic file type, used in file io functions, if =NULL then it is either invalid or not created
 * @ingroup fileio
 */
typedef void* file_t;

/**
 * @see fio_seek @ingroup fileio
 */
enum seek_mode
{
    SEEK_MODE_START,    /**< seek from the beginning of the file */
    SEEK_MODE_END, /**< seek from the end of the file */
    SEEK_MODE_CUR /**< seek from the current position of the file */
};

/**
 * @ingroup fileio
 */
enum file_type
{
    FILE_TYPE_MEM, /**< file resides in memory */
    FILE_TYPE_DSK /**< file resides on disk */
};

/**
 * @ingroup fileio
 */
enum file_mode
{
    FILE_MODE_WRITE, /**< file is opened for writing */
    FILE_MODE_READ /**< file is opened for reading */
};

/* init/release manager */
result_t fio_initmgr();
void fio_releasemgr();

/**
 * Add virtual-directory to the virtual-filesystem \n
 * Example usage:\n
 * @code
 * fio_addvdir("d:\\mygame\\data\\");
 * fio_opendisk("textures\\hello.dds"); // opens file from d:\mygame\data\textures\hello.dds
 * @endcode
 * @param directory directory on the disk to add to root directories of virtual-filesystem
 * @param monitor Enables file monitoring for the entire directory files and it's subtree
 * (Requires @e _FILEMON_ compiler preprocessor)
 * @ingroup fileio
 */
CORE_API int fio_addvdir(const char* directory, int monitor);

/**
 * Clears virtual directories
 * @ingroup fileio
 */
CORE_API void fio_clearvdirs();

/**
 * Add/clear pak files to the virtual-filesystems\n
 * Files inside pak-file behaves like a virtual-disk, and are referenced same as virtual-directories\n
 * **Note** handling of opening and closing the pak-files must be managed by user
 * @see pak_file
 * @see fio_addvdir
 * @ingroup fileio
 */
CORE_API void fio_addpak(struct pak_file* pak);

/**
 * Clear pak files list in the virtual filesystem
 * @ingroup fileio
 */
CORE_API void fio_clearpaks();

 /**
  * Create a file in memory
  * @param alloc memory allocator for internal file data
  * @param name name alias (or filepath) that will be binded to the file
  * @return valid file handle or NULL if failed
  * @ingroup fileio
  */
CORE_API file_t fio_createmem(struct allocator* alloc, const char* name, uint mem_id);

/**
 * Open a file from disk into memory
 * @param filepath filepath to the file on disk (must exist), filepath will first check -
 * virtual-filesystems for valid path unless ignore_vfs option is set
 * @param ignore_vfs if true, virtual-filesystems will be ignored and file will be loaded directly
 * @return valid file handle or NULL if failed
 * @ingroup fileio
  */
CORE_API file_t fio_openmem(struct allocator* alloc, const char* filepath, int ignore_vfs,
                            uint mem_id);
/**
 * Attach a memory buffer to the file for reading, attached buffer should not be -
 * managed (deallocated) by caller anymore
 * @param alloc allocator that used to create the buffer
 * @param buffer buffer that we want to attach
 * @param name name alias (or filepath) that will be binded to the file
 * @return valid file handle or NULL if failed
 * @ingroup fileio
  */
CORE_API file_t fio_attachmem(struct allocator* alloc, void* buffer, size_t size, const char* name,
                              uint mem_id);


CORE_API char* fio_loadtext(struct allocator *alloc, const char *filepath, int ignore_vfs,
                            uint mem_id, OUT OPTIONAL size_t *size);

/**
 * Detach buffer from memory file, after file is detached\n
 * Note that file is *NOT* closed after detaching it's memory, user must close it after detach
 * @param outsize output size of the buffer
 * @param palloc pointer (out) to the allocator that used to create file buffer
 * @return file data buffer, caller should manage freeing the buffer
 * @ingroup fileio
 */
CORE_API void* fio_detachmem(file_t f, size_t* outsize, struct allocator** palloc);

/**
 * Create a file on disk
 * @param ignore_vfs sets if we have to ignore opening from virtual-filesystems
 * @return valid file handle or NULL if failed
 * @ingroup fileio
 */
CORE_API file_t fio_createdisk(const char* filepath);

/**
 * Opens a file from disk (must exist), filepath will first check virtual-filesystems for valid -
 * path unless ignore_vfs option is set
 * @param ignore_vfs if true, virtual-filesystems will be ignored and file will be loaded directly
 * @return valid file handle or NULL if failed
 * @ingroup fileio
 */
CORE_API file_t fio_opendisk(const char* filepath, int ignore_vfs);

/**
 * Close an opened file
 * @ingroup fileio
 */
CORE_API void fio_close(file_t f);

/**
 * Seeks in the file
 * @param seek defines where should seeking begin (see seek_mode)
 * @param offset offset from the seek point to move in the file, can be positive/negative
 * @ingroup fileio
 */
CORE_API int fio_seek(file_t f, enum seek_mode seek, int offset);

/**
 * Reads data from file
 * @param buffer output buffer, buffer should have enough size of (item_size*items_cnt)
 * @param item_size size of each item in bytes
 * @param items_cnt number of items to read
 * @return number of items that is read, =0 if error occurs
 * @ingroup fileio
 */
CORE_API size_t fio_read(file_t f, void* buffer, size_t item_size, size_t items_cnt);

/**
 * Writes data to file
 * @param buffer input buffer, buffer should have enough size of (item_size*items_cnt)
 * @param item_size size of each item in bytes
 * @param items_cnt number of items to read
 * @return number of items that is written to file, =0 if error occurs
 * @ingroup fileio
*/
CORE_API size_t fio_write(file_t f, const void* buffer, size_t item_size, size_t items_cnt);

/**
 * @return File size in bytes
 * @ingroup fileio
 */
CORE_API size_t fio_getsize(file_t f);

/**
 * @return Current position offset in the file (in bytes)
 * @ingroup fileio
 */
CORE_API size_t fio_getpos(file_t f);

/**
 * Returns file-path, the one that is called with fio_openXXX functions
 * @ingroup fileio
 */
CORE_API const char* fio_getpath(file_t f);

/**
 * Checks if file is already opened
 * @ingroup fileio
 */
CORE_API int fio_isopen(file_t f);

/**
 * Get opened file type
 * @see file_type
 * @ingroup fileio
 */
CORE_API enum file_type fio_gettype(file_t f);
/**
 * get opened file mode (read/write)
 * @see FILE_MODE_READ
 * @ingroup fileio
 */
CORE_API enum file_mode fio_getmode(file_t f);

/**
 * @ingroup fileio
 */
typedef void (*pfn_fio_modify)(const char* filepath, reshandle_t hdl, uptr_t param1,
    uptr_t param2);

/**
 * @ingroup fileio
 */
CORE_API void fio_mon_reg(const char* filepath, pfn_fio_modify fn, reshandle_t hdl,
    uptr_t param1, uptr_t param2);

/**
 * @ingroup fileio
 */
CORE_API void fio_mon_unreg(const char* filepath);

/**
 * @ingroup fileio
 */
CORE_API void fio_mon_update();

/**
 * @ingroup fileio
 */
CORE_API int fio_mon_avail();

#ifdef __cplusplus
#include "err.h"

namespace dh {

class File
{
private:
    file_t m_file;

public:
    struct mem_data
    {
        void *buff;
        size_t size;
        allocator *alloc;
    };

public:
    File() : m_file(NULL) {}
    File(file_t f) : m_file(f) {}

    virtual ~File()
    {
        if (m_file != NULL)
            fio_close(m_file);
    }

    static File create_mem(const char* alias, allocator *alloc = mem_heap(), uint mem_id = 0)
    {
        return File(fio_createmem(alloc, alias, mem_id));
    }

    static File create_disk(const char *filepath)
    {
        return File(fio_createdisk(filepath));
    }

    static File open_mem(const char *filepath, allocator *alloc = mem_heap(), uint mem_id = 0,
                           bool ignore_vfs = false)
    {
        return File(fio_openmem(alloc, filepath, ignore_vfs, mem_id));
    }

    static File open_disk(const char *filepath, bool ignore_vfs = false)
    {
        return File(fio_opendisk(filepath, ignore_vfs));
    }

    static File attach_mem(void *buff, size_t size, const char *alias,
                             allocator *alloc = mem_heap(), uint mem_id = 0)
    {
        return File(fio_attachmem(alloc, buff, size, alias, mem_id));
    }

    void seek(int offset, seek_mode smode = SEEK_MODE_START)
    {
        ASSERT(m_file);
        fio_seek(m_file, smode, offset);
    }

    void read(void *buff, size_t item_sz, size_t item_cnt)
    {
        ASSERT(m_file);
        fio_read(m_file, buff, item_sz, item_cnt);
    }

    void write(const void *buff, size_t item_sz, size_t item_cnt)
    {
        ASSERT(m_file);
        fio_write(m_file, buff, item_sz, item_cnt);
    }

    mem_data detach_mem()
    {
        mem_data fm;
        fm.buff = fio_detachmem(m_file, &fm.size, &fm.alloc);
        return fm;
    }

    file_mode mode() const
    {
        ASSERT(m_file);
        return fio_getmode(m_file);
    }

    size_t size() const
    {
        ASSERT(m_file);
        return fio_getsize(m_file);
    }

    file_type type() const
    {
        ASSERT(m_file);
        return fio_gettype(m_file);
    }

    void close()
    {
        if (m_file != NULL) {
            fio_close(m_file);
            m_file = NULL;
        }
    }

    bool is_open() const
    {
        return m_file != NULL;
    }

    operator file_t()
    {
        return m_file;
    }
};

} /* dh */
#endif

#endif /* __FILEIO_H__*/
