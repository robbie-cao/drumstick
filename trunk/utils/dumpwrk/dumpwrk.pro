TEMPLATE = app
TARGET = drumstick-dumpwrk
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += qt console link_prl
DESTDIR = ../../build/bin
INCLUDEPATH += . ../../library/include ../common ../../build/common
LIBS = -L../../build/lib -ldrumstick-common -ldrumstick-file
PRE_TARGETDEPS += $$OUT_PWD/../../build/lib/libdrumstick-common.a
include (../../global.pri)
# Input
HEADERS += dumpwrk.h
SOURCES += dumpwrk.cpp