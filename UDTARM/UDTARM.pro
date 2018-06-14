QT -= gui
QT += multimedia
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    udt4/src/api.cpp \
    udt4/src/buffer.cpp \
    udt4/src/cache.cpp \
    udt4/src/ccc.cpp \
    udt4/src/channel.cpp \
    udt4/src/common.cpp \
    udt4/src/core.cpp \
    udt4/src/epoll.cpp \
    udt4/src/list.cpp \
    udt4/src/md5.cpp \
    udt4/src/packet.cpp \
    udt4/src/queue.cpp \
    udt4/src/window.cpp \
    zservicethread.cpp

HEADERS += \
    udt4/src/api.h \
    udt4/src/buffer.h \
    udt4/src/cache.h \
    udt4/src/ccc.h \
    udt4/src/channel.h \
    udt4/src/common.h \
    udt4/src/core.h \
    udt4/src/epoll.h \
    udt4/src/list.h \
    udt4/src/md5.h \
    udt4/src/packet.h \
    udt4/src/queue.h \
    udt4/src/udt.h \
    udt4/src/window.h \
    zservicethread.h

DEFINES += __MINGW__
LIBS += -lwsock32 -lws2_32

INCLUDEPATH += C:/opencv2410forMinGw/include
LIBS += C:/opencv2410forMinGw/lib/libopencv_core2410.dll.a \
        C:/opencv2410forMinGw/lib/libopencv_imgproc2410.dll.a \
        C:/opencv2410forMinGw/lib/libopencv_highgui2410.dll.a
