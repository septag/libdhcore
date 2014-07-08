TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += warn_off

INCLUDEPATH = ../../include

DEFINES += _VERSION_=\\\"1.0\\\"

linux-g++|linux-clang   {
    QMAKE_CFLAGS += \
        -std=gnu99 \
        -msse -msse2 -msse4.1 \
        -ffast-math

    LIBS *= -lpthread -lm
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
