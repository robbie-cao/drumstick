# MIDI Sequencer C++ Library
# Copyright (C) 2005-2016 Pedro Lopez-Cabanillas <plcl@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA

SET(dumpmid_SRCS
    dumpmid.cpp
    dumpmid.h
)

SET(dumpmid_qtobject_SRCS
    dumpmid.h
)

QT5_WRAP_CPP(dumpmid_moc_SRCS ${dumpmid_qtobject_SRCS})

ADD_EXECUTABLE(drumstick-dumpmid
    ${dumpmid_moc_SRCS}
    ${dumpmid_SRCS}
)

TARGET_LINK_LIBRARIES(drumstick-dumpmid
    ${ALSA_LIBS} 
    drumstick-common
    drumstick-alsa
    Qt5::Core
)

# we should install some files...
INSTALL(TARGETS drumstick-dumpmid 
        RUNTIME DESTINATION bin)
