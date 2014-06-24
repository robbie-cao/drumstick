TEMPLATE = lib
CONFIG += plugin static create_prl
TARGET = $$qtLibraryTarget(drumstick-rt-alsa)
DESTDIR = ../../../build/lib
DEPENDPATH += . ../../include
INCLUDEPATH += . ../../include
include (../../../global.pri)
DEPENDPATH += ../../include
INCLUDEPATH += ../../include
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += ../../include/rtmidiinput.h \
           ../../include/rtmidioutput.h \
           alsamidiinput.h \
           alsamidioutput.h
SOURCES += alsamidiinput.cpp \
           alsamidioutput.cpp

#CONFIG += link_pkgconfig
#PKGCONFIG += drumstick-alsa
