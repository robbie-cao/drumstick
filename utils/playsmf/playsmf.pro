TEMPLATE = app
TARGET = drumstick-playsmf
QT += dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += qt console thread exceptions
DESTDIR = ../../build/bin
OBJECTS_DIR = ../../build/obj
MOC_DIR = ../../build/moc
INCLUDEPATH += . ../../library/include ../common ../../build/common
LIBS = -L../../build/lib -ldrumstick-common -ldrumstick-alsa -ldrumstick-file -lasound
include (../../global.pri)
# Input
HEADERS += playsmf.h
SOURCES += playsmf.cpp