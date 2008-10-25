/*
    SMF GUI Player test using the MIDI Sequencer C++ library 
    Copyright (C) 2006-2008, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along 
    with this program; if not, write to the Free Software Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.    
*/

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <cmath>

#include "smfplayer.h"

SMFPlayer::SMFPlayer(QWidget *parent)
    : QWidget(parent),
    m_portId(-1),
    m_queueId(-1),
    m_initialTempo(0),
    m_tempoFactor(1.0),
    m_tick(0)
{
	ui.setupUi(this);
    connect(ui.btnPlay, SIGNAL(clicked()), SLOT(play()));
    connect(ui.btnPause, SIGNAL(clicked()), SLOT(pause()));
    connect(ui.btnStop, SIGNAL(clicked()), SLOT(stop()));
    connect(ui.btnOpen, SIGNAL(clicked()), SLOT(open()));
    connect(ui.btnSetup, SIGNAL(clicked()), SLOT(setup()));
    connect(ui.btnReset, SIGNAL(clicked()), SLOT(tempoReset()));
    connect(ui.sliderTempo, SIGNAL(valueChanged(int)), SLOT(tempoSlider(int)));
	connect(ui.btnExit, SIGNAL(clicked()), SLOT(quit()));

    m_Client = new MidiClient(this);
    m_Port = new MidiPort(this);

    m_Client->setOpenMode(SND_SEQ_OPEN_DUPLEX);
    m_Client->setBlockMode(false);
    m_Client->open();
    m_Client->setPoolOutput(100);
    m_Client->setClientName("MIDI Player");
    connect(m_Client, SIGNAL(eventReceived(SequencerEvent*)), 
                      SLOT(sequencerEvent(SequencerEvent*)));

    m_Port->setMidiClient(m_Client);
    m_Port->setPortName("MIDI Player port");
    m_Port->setCapability( SND_SEQ_PORT_CAP_READ  | 
                           SND_SEQ_PORT_CAP_SUBS_READ | 
                           SND_SEQ_PORT_CAP_WRITE );
    m_Port->setPortType(SND_SEQ_PORT_TYPE_APPLICATION);
    m_Port->attach();

    m_Queue = m_Client->createQueue("SMFPlayer");
    m_queueId = m_Queue->getId();
    m_portId = m_Port->getPortId();

    m_engine = new QSmf(this);
    connect(m_engine, SIGNAL(signalSMFHeader(int,int,int)), SLOT(headerEvent(int,int,int)));
    connect(m_engine, SIGNAL(signalSMFNoteOn(int,int,int)), SLOT(noteOnEvent(int,int,int)));
    connect(m_engine, SIGNAL(signalSMFNoteOff(int,int,int)), SLOT(noteOffEvent(int,int,int)));
    connect(m_engine, SIGNAL(signalSMFKeyPress(int,int,int)), SLOT(keyPressEvent(int,int,int)));
    connect(m_engine, SIGNAL(signalSMFCtlChange(int,int,int)), SLOT(ctlChangeEvent(int,int,int)));
    connect(m_engine, SIGNAL(signalSMFPitchBend(int,int)), SLOT(pitchBendEvent(int,int)));
    connect(m_engine, SIGNAL(signalSMFProgram(int,int)), SLOT(programEvent(int,int)));
    connect(m_engine, SIGNAL(signalSMFChanPress(int,int)), SLOT(chanPressEvent(int,int)));
    connect(m_engine, SIGNAL(signalSMFSysex(const QByteArray&)), SLOT(sysexEvent(const QByteArray&)));
    connect(m_engine, SIGNAL(signalSMFText(int,const QString&)), SLOT(textEvent(int,const QString&)));
    connect(m_engine, SIGNAL(signalSMFTempo(int)), SLOT(tempoEvent(int)));
    connect(m_engine, SIGNAL(signalSMFError(const QString&)), SLOT(errorHandler(const QString&)));
    
    m_player = new Player(m_Client, m_portId);
    connect(m_player, SIGNAL(finished()), SLOT(songFinished()));

    m_Client->startSequencerInput();
}

SMFPlayer::~SMFPlayer()
{
    m_Client->stopSequencerInput();
    m_Port->detach();
    m_Client->close();
    delete m_player;
}

void SMFPlayer::subscribe(const QString& portName)
{
    try {
        if (!m_subscription.isEmpty()) {
            m_Port->unsubscribeTo(m_subscription);
        }
        m_subscription = portName;
        m_Port->subscribeTo(m_subscription);
    } catch (SequencerError& err) {
        qDebug() << "SequencerError exception. Error code: " << err.code() 
                 << " (" << err.qstrError() << ")";
        qDebug() << "Location: " << err.location();
    }
}

void SMFPlayer::updateTimeLabel(int mins, int secs, int cnts)
{
    static QChar fill('0');
    QString stime = QString("%1:%2.%3").arg(mins,2,10,fill)
                                       .arg(secs,2,10,fill)
                                       .arg(cnts,2,10,fill);
    ui.lblTime->setText(stime);
}

void SMFPlayer::play()
{
    if (m_initialTempo == 0) 
        return;
    if (m_player->getInitialPosition() == 0) {
        QueueTempo firstTempo = m_Queue->getTempo();
        firstTempo.setPPQ(m_song.getDivision());
        firstTempo.setTempo(m_initialTempo);
        firstTempo.setTempoFactor(m_tempoFactor);
        m_Queue->setTempo(firstTempo);
        m_Client->drainOutput();
    }
    m_player->start();
}

void SMFPlayer::pause()
{
    if (m_player->isRunning()) {
        m_player->stop();
        m_player->setPosition(m_Queue->getStatus().getTickTime());
    }
}

void SMFPlayer::stop()
{
    if (m_initialTempo != 0) {
        m_player->stop();
        m_player->resetPosition();
        updateTimeLabel(0,0,0);
        updateTempoLabel(6.0e7f / m_initialTempo);
    }
}

void SMFPlayer::open()
{
    QFileDialog dlg(this, "Open MIDI File",
                    NULL, "MIDI Files (*.mid);;Karaoke files (*.kar)");
    
    if (dlg.exec())
    {
        QStringList fileNames = dlg.selectedFiles();
        QString firstName = fileNames.takeFirst();
        m_song.clear();
        m_tick = 0;
        m_initialTempo = 0;
        m_engine->readFromFile(firstName);
        m_song.sort();
        m_player->setSong(&m_song);
        if (m_initialTempo == 0) {
            m_initialTempo = 500000;
        }
        QFileInfo fi(firstName);
        ui.lblName->setText(fi.fileName());
        ui.lblCopyright->setText(m_song.getCopyright());
        ui.progressBar->setValue(0);
        updateTimeLabel(0,0,0);
        updateTempoLabel(6.0e7f / m_initialTempo);
    }
}

void SMFPlayer::setup()
{
    bool ok;
    int current;
    QStringList items;
    QListIterator<PortInfo> it(m_Client->getAvailableOutputs());
    while(it.hasNext()) {
        PortInfo p = it.next();
        items << QString("%1:%2").arg(p.getClientName()).arg(p.getPort());
    }
    current = items.indexOf(m_subscription);
    QString item = QInputDialog::getItem(this, "Player subscription", 
                                         "Output port:", items, 
                                         current, false, &ok);
    if (ok && !item.isEmpty()) {
        subscribe(item);
    }
}

void SMFPlayer::songFinished()
{
    m_player->resetPosition();
    ui.btnStop->setChecked(true);
    updateTimeLabel(0,0,0);    
}

void SMFPlayer::updateTempoLabel(float ftempo)
{
    QString stempo = QString("%1 bpm").arg(ftempo, 0, 'f', 2);
    ui.lblOther->setText(stempo);
}

void SMFPlayer::sequencerEvent(SequencerEvent *ev)
{
    if (ev->getSequencerType() == SND_SEQ_EVENT_ECHO) {
        int pos = 100 * ev->getTick() / m_tick;
        const snd_seq_real_time_t* rt = m_Queue->getStatus().getRealtime();
        int mins = rt->tv_sec / 60; 
        int secs =  rt->tv_sec % 60;
        int cnts = floor( rt->tv_nsec / 1.0e7 );
        updateTempoLabel(m_Queue->getTempo().getRealBPM());
        updateTimeLabel(mins, secs, cnts);
        ui.progressBar->setValue(pos);
    }
    delete ev;
}

void SMFPlayer::appendEvent(SequencerEvent* ev)
{
    unsigned long tick = m_engine->getCurrentTime();
    ev->setSource(m_portId);
    if (ev->getSequencerType() != SND_SEQ_EVENT_TEMPO) {
        ev->setSubscribers();
    }
    ev->scheduleTick(m_queueId, tick, false);
    m_song.append(ev);
    if (tick > m_tick) m_tick = tick;
}

void SMFPlayer::headerEvent(int format, int ntrks, int division)
{
    m_song.setHeader(format, ntrks, division);
}

void SMFPlayer::noteOnEvent(int chan, int pitch, int vol)
{
    SequencerEvent* ev = new NoteOnEvent (chan, pitch, vol);
    appendEvent(ev);
}

void SMFPlayer::noteOffEvent(int chan, int pitch, int vol)
{
    SequencerEvent* ev = new NoteOffEvent (chan, pitch, vol);
    appendEvent(ev);
}

void SMFPlayer::keyPressEvent(int chan, int pitch, int press)
{
    SequencerEvent* ev = new KeyPressEvent (chan, pitch, press);
    appendEvent(ev);
}

void SMFPlayer::ctlChangeEvent(int chan, int ctl, int value)
{
    SequencerEvent* ev = new ControllerEvent (chan, ctl, value);
    appendEvent(ev);
}

void SMFPlayer::pitchBendEvent(int chan, int value)
{
    SequencerEvent* ev = new PitchBendEvent (chan, value);
    appendEvent(ev);
}

void SMFPlayer::programEvent(int chan, int patch)
{
    SequencerEvent* ev = new ProgramChangeEvent (chan, patch);
    appendEvent(ev);
}

void SMFPlayer::chanPressEvent(int chan, int press)
{
    SequencerEvent* ev = new ChanPressEvent (chan, press);
    appendEvent(ev);
}

void SMFPlayer::sysexEvent(const QByteArray& data)
{
    SequencerEvent* ev = new SysExEvent (data);
    appendEvent(ev);
}

void SMFPlayer::textEvent(int type, const QString& data)
{
    m_song.addText(type, data);
}

void SMFPlayer::tempoEvent(int tempo)
{
    if ( m_initialTempo == 0 )
    {
        m_initialTempo = tempo;
    }
    SequencerEvent* ev = new TempoEvent (m_queueId, tempo);
    appendEvent(ev);
}

void SMFPlayer::errorHandler(const QString& errorStr)
{
    qDebug() << errorStr;
    exit(1);
}

void SMFPlayer::tempoReset()
{
    ui.sliderTempo->setValue(100);
    m_tempoFactor = 1.0;
}

void SMFPlayer::tempoSlider(int value)
{
    m_tempoFactor = value / 100.0;
    QueueTempo qtempo = m_Queue->getTempo();
    qtempo.setTempoFactor(m_tempoFactor);
    m_Queue->setTempo(qtempo);
    m_Client->drainOutput();
    if (!m_player->isRunning()) {
        updateTempoLabel(qtempo.getRealBPM());
    }
}

void SMFPlayer::quit()
{
    stop();
    m_player->wait();
    QApplication::quit();
}
