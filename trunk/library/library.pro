TEMPLATE = subdirs
CONFIG += ordered
linux* {
    SUBDIRS += alsa
}
SUBDIRS += file
SUBDIRS += rt
