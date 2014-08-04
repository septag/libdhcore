TEMPLATE = lib
CONFIG += shared
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= warn_on
VERSION = "1.0.0"

DEFINES += _VERSION_=\\\"$$VERSION\\\"

CONFIG(release, debug|release) {
    TARGET = dhcore
}

CONFIG(debug, debug|release) {
    TARGET = dhcore-dbg
    DEFINES += _ENABLEASSERT_ _DEBUG_
}

INCLUDEPATH = \
    ../../include/dhcore \
    ../../include \
    deps

DEFINES += _CORE_EXPORT_ _SIMD_SSE_

linux-g++|linux-clang   {
    QMAKE_CFLAGS += \
        -std=gnu99 \
        -msse -msse2 -msse4.1 \
        -ffast-math

    LIBS *= -lpthread -lm
}

SOURCES += \
    array.c \
    core.c \
    errors.c \
    file-io.c \
    freelist-alloc.c \
    hash.c \
    hash-table.c \
    hwinfo.c \
    json.c \
    log.c \
    mem-mgr.c \
    net-socket.c \
    numeric.c \
    pak-file.c \
    pool-alloc.c \
    prims.c \
    rpc.c \
    stack-alloc.c \
    std-math.c \
    str.c \
    task-mgr.c \
    timer.c \
    util.c \
    variant.c \
    vec-math.c \
    zip.c \
    deps/cJSON/cJSON.c \
    deps/commander/commander.c \
    deps/miniz/miniz.c \
    path.c \
    static-vars.cpp

unix    {
    SOURCES += \
        platform/posix/crash-posix.c \
        platform/posix/mt-posix.c \
        platform/posix/util-posix.c
}

win32   {
    SOURCES += \
        platform/win/hwinfo-win.c \
        platform/win/mt-win.c \
        platform/win/timer-win.c \
        platform/win/util-win.c \
        platform/win/crash-win.cpp
    DEFINES += _WIN_
}

macx {
    SOURCES += \
        platform/osx/hwinfo-osx.c \
        platform/osx/timer-osx.c \
        platform/osx/util-osx.c
    DEFINES += _OSX_
}

# linux (very likely)
unix:!macx  {
    SOURCES += \
        platform/linux/hwinfo-lnx.c \
        platform/linux/timer-lnx.c \
        platform/linux/util-lnx.c
    DEFINES += _LINUX_
    DEFINES += HAVE_MALLOC_H
}

HEADERS = \
    ../../include/dhcore/allocator.h \
    ../../include/dhcore/array.h \
    ../../include/dhcore/color.h \
    ../../include/dhcore/commander.h \
    ../../include/dhcore/core-api.h \
    ../../include/dhcore/core.h \
    ../../include/dhcore/crash.h \
    ../../include/dhcore/err.h \
    ../../include/dhcore/error-codes.h \
    ../../include/dhcore/file-io.h \
    ../../include/dhcore/freelist-alloc.h \
    ../../include/dhcore/hash-table.h \
    ../../include/dhcore/hash.h \
    ../../include/dhcore/hwinfo.h \
    ../../include/dhcore/json.h \
    ../../include/dhcore/linked-list.h \
    ../../include/dhcore/log.h \
    ../../include/dhcore/mem-mgr.h \
    ../../include/dhcore/mt.h \
    ../../include/dhcore/net-socket.h \
    ../../include/dhcore/numeric.h \
    ../../include/dhcore/pak-file-fmt.h \
    ../../include/dhcore/pak-file.h \
    ../../include/dhcore/pool-alloc.h \
    ../../include/dhcore/prims.h \
    ../../include/dhcore/queue.h \
    ../../include/dhcore/rpc.h \
    ../../include/dhcore/stack-alloc.h \
    ../../include/dhcore/stack.h \
    ../../include/dhcore/std-math.h \
    ../../include/dhcore/str.h \
    ../../include/dhcore/task-mgr.h \
    ../../include/dhcore/timer.h \
    ../../include/dhcore/types.h \
    ../../include/dhcore/util.h \
    ../../include/dhcore/variant.h \
    ../../include/dhcore/vec-math.h \
    ../../include/dhcore/win.h \
    ../../include/dhcore/zip.h \
    ../../include/dhcore/path.h

