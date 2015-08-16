TEMPLATE = lib
TARGET = drumstick-alsa
DESTDIR = ../build/lib
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
DEPENDPATH += . include src
INCLUDEPATH += . include
include (../global.pri)
CONFIG += qt staticlib debug
DEFINES += drumstick_alsa_EXPORTS
# Input
HEADERS += include/alsaclient.h \
           include/alsaevent.h \
           include/alsaport.h \
           include/alsaqueue.h \
           include/alsatimer.h \
           include/drumstick.h \
           include/drumstickcommon.h \
           include/playthread.h \
           include/subscription.h
SOURCES += src/alsaclient.cpp \
           src/alsaevent.cpp \
           src/alsaport.cpp \
           src/alsaqueue.cpp \
           src/alsatimer.cpp \
           src/playthread.cpp \
           src/subscription.cpp