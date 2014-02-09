/*
    Virtual Piano Widget for Qt4
    Copyright (C) 2008-2013, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include <QFont>
#include "keylabel.h"

KeyLabel::KeyLabel(QGraphicsItem *parent) : QGraphicsTextItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    rotate(270);
#else
    setRotation(270);
#endif
}