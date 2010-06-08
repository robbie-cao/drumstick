TEMPLATE = app
TARGET = drumstick-timertest
DESTDIR = ../../build/bin
OBJECTS_DIR = ../../build/obj
MOC_DIR = ../../build/moc
INCLUDEPATH += . ../../library/include ../common ../../build/common
DEPENDPATH += . ../../library ../../library/include
LIBS = -L../../build/lib -ldrumstick-alsa  -lasound
# Input
HEADERS = timertest.h
SOURCES = timertest.cpp ../common/cmdlineargs.cpp
