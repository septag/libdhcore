TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += warn_off

INCLUDEPATH = ../../include

DEFINES += _VERSION_=\\\"1.0\\\"

linux-g++|linux-clang|macx-clang   {
    QMAKE_CFLAGS += \
        -std=gnu99 \
        -msse -msse2 \
        -ffast-math
    QMAKE_CXXFLAGS += \
        -msse -msse2 \
        -ffast-math \
        -std=c++11
    LIBS *= -lpthread -lm
}

win32-msvc2013 | win32-msvc2012 | win32-msvc2010 {
    QMAKE_CFLAGS += /TP

    CONFIG(debug, debug|release)    {
        DEFINES += _DEBUG
    }

    DEFINES += _CRT_SECURE_NO_WARNINGS _WINDOWS _WINDLL _MBCS
    DEFINES -= UNICODE

    LIBS += -lws2_32 -lShlwapi -luser32 -lgdi32 -lkernel32 -lAdvapi32 -lShell32
}

SOURCES += \
    dhcore-test.c \
    test-freelist.c \
    test-heap.c \
    test-json.c \
    test-pool.c \
    test-taskmgr.c \
    test-thread.c \
    test-hashtable.cpp

HEADERS += \
    dhcore-test.h

# dhcore lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -ldhcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -ldhcore
else:unix:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/ -ldhcore
else:unix:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/ -ldhcore-dbg

DEPENDPATH += $$PWD/../core
