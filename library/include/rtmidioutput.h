/*
    Drumstick MIDI realtime input-output
    Copyright (C) 2009-2013 Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef MIDIOUTPUT_H
#define MIDIOUTPUT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtPlugin>

namespace drumstick {
namespace rt {

    /**
     * Virtual MIDI OUT port
     */
    class MIDIOutput : public QObject
    {
        Q_OBJECT

    public:
        explicit MIDIOutput(QObject *parent = 0) : QObject(parent) {}
        virtual ~MIDIOutput() {}

        /**
         * @brief backendName
         * @return plugin name
         */
        virtual QString backendName() = 0;
        /**
         * @brief publicName
         * @return MIDI port name
         */
        virtual QString publicName() = 0;
        /**
         * @brief setPublicName
         * @param name MIDI port name
         */
        virtual void setPublicName(QString name) = 0;
        /**
         * @brief connections
         * @return list of available MIDI ports
         */
        virtual QStringList connections(bool advanced = false) = 0;
        /**
         * @brief setExcludedConnections
         * @param conns
         */
        virtual void setExcludedConnections(QStringList conns) = 0;
        /**
         * @brief open the MIDI port by name
         * @param name
         */
        virtual void open(QString name) = 0;
        /**
         * @brief close the MIDI port
         */
        virtual void close() = 0;
        /**
         * @brief currentConnection
         * @return name of the current connection if it is opened
         */
        virtual QString currentConnection() = 0;

    public Q_SLOTS:
        /**
         * @brief sendNoteOff 0x8
         * @param chan
         * @param note
         * @param vel
         */
        virtual void sendNoteOff(int chan, int note, int vel) = 0;

        /**
         * @brief sendNoteOn 0x9
         * @param chan
         * @param note
         * @param vel
         */
        virtual void sendNoteOn(int chan, int note, int vel) = 0;

        /**
         * @brief sendKeyPressure 0xA
         * @param chan
         * @param note
         * @param value
         */
        virtual void sendKeyPressure(int chan, int note, int value) = 0;

        /**
         * @brief sendController 0xB
         * @param chan
         * @param control
         * @param value
         */
        virtual void sendController(int chan, int control, int value) = 0;

        /**
         * @brief sendProgram 0xC
         * @param chan
         * @param program
         */
        virtual void sendProgram(int chan, int program) = 0;

        /**
         * @brief sendChannelPressure 0xD
         * @param chan
         * @param value
         */
        virtual void sendChannelPressure(int chan, int value) = 0;

        /**
         * @brief sendPitchBend 0xE
         * @param chan
         * @param value
         */
        virtual void sendPitchBend(int chan, int value) = 0;

        /**
         * @brief sendSysex
         * @param data 0xF0 ... 0xF7
         */
        virtual void sendSysex(const QByteArray& data) = 0;

        /**
         * @brief sendSystemMsg
         * @param status 0xF
         */
        virtual void sendSystemMsg(const int status) = 0;
    };
}
}

Q_DECLARE_INTERFACE(drumstick::rt::MIDIOutput, "net.sourceforge.drumstick.rt.MIDIOutput/0.1")

#endif /* MIDIOUTPUT_H */