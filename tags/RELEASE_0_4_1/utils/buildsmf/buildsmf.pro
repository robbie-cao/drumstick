TEMPLATE = app
TARGET = drumstick-buildsmf
DESTDIR = ../../build/bin
OBJECTS_DIR = ../../build/obj
MOC_DIR = ../../build/moc
INCLUDEPATH += . ../../library/include ../common ../../build/common
LIBS = -L../../build/lib -ldrumstick-common -ldrumstick-file
# Input
HEADERS += buildsmf.h
SOURCES += buildsmf.cpp