/*
    MIDI Sequencer C++ library
    Copyright (C) 2006-2009, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef TIMERTEST_H_
#define TIMERTEST_H_

#include <QObject>
#include <QPointer>
#include "alsatimer.h"

using namespace ALSA;

class TimerTest : public QObject, public TimerEventHandler
{
    Q_OBJECT

public:
    TimerTest(QObject* parent = 0): QObject(parent) {};
    virtual ~TimerTest() {};

    void queryTimers();
    void showStatus();
    void showInfo();
    void runTest();

    // TimerEventHandler implementation
    void handleTimerEvent(int ticks, int msecs);

private:
    TimerInfo   m_info;
    TimerStatus m_status;
    TimerParams m_params;
    QPointer<Timer> m_timer;
};

#endif /*TIMERTEST_H_*/