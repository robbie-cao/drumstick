TEMPLATE = app
TARGET = drumstick-sysinfo
QT += dbus
CONFIG += qt console thread exceptions
DESTDIR = ../../build/bin
OBJECTS_DIR = ../../build/obj
MOC_DIR = ../../build/moc
INCLUDEPATH += . ../../library/include ../common ../../build/common
DEPENDPATH += . ../../library ../../library/include
LIBS = -L../../build/lib -ldrumstick-common -ldrumstick-alsa -lasound
# Input
SOURCES += sysinfo.cpp