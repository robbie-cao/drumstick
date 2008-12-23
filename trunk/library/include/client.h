/*
    MIDI Sequencer C++ library 
    Copyright (C) 2006-2008, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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
 
#ifndef INCLUDED_CLIENT_H
#define INCLUDED_CLIENT_H

#include "commons.h"
#include "port.h"
#include <QObject>
#include <QPointer>
#include <QList>
#include <QThread>
#include <QReadWriteLock>

namespace ALSA 
{
namespace Sequencer 
{

class MidiQueue;
class MidiClient;
class SequencerEvent;
class SequencerInputThread;
class RemoveEvents;

class ClientInfo
{
    friend class MidiClient;

public:
    ClientInfo();
    ClientInfo(const ClientInfo& other);
    ClientInfo(snd_seq_client_info_t* other); 
    ClientInfo(MidiClient* seq, int id);
    virtual ~ClientInfo();
    ClientInfo* clone();
    ClientInfo& operator=(const ClientInfo& other);
    int getSizeOfInfo() const;

    int getClientId();
    snd_seq_client_type_t getClientType();
    QString getName();
    bool getBroadcastFilter();
    bool getErrorBounce();
    int getNumPorts();
    int getEventLost();
    void setClient(int client);
    void setName(QString name);
    void setBroadcastFilter(bool val);
    void setErrorBounce(bool val);
    PortInfoList getPorts() const;

#if SND_LIB_SUBMINOR > 16    
    void addFilter(int eventType);
    bool isFiltered(int eventType);
    void clearFilter();
    void removeFilter(int eventType);
#endif

protected:    
    void readPorts(MidiClient* seq);
    void freePorts();

    const unsigned char* getEventFilter() __attribute__((deprecated));
    void setEventFilter(unsigned char* filter) __attribute__((deprecated));

private:
    snd_seq_client_info_t* m_Info;
    PortInfoList m_Ports;
};

typedef QList<ClientInfo> ClientInfoList;

class SystemInfo
{
    friend class MidiClient;

public:
    SystemInfo();
    SystemInfo(const SystemInfo& other);
    SystemInfo(snd_seq_system_info_t* other); 
    SystemInfo(MidiClient* seq);
    virtual ~SystemInfo();
    SystemInfo* clone();
    SystemInfo& operator=(const SystemInfo& other);
    int getSizeOfInfo() const;
    
    int getMaxClients();
    int getMaxPorts();
    int getMaxQueues();
    int getMaxChannels();
    int getCurrentQueues();
    int getCurrentClients();

private:
    snd_seq_system_info_t* m_Info;
};

class PoolInfo
{
    friend class MidiClient;

public:
    PoolInfo();
    PoolInfo(const PoolInfo& other);
    PoolInfo(snd_seq_client_pool_t* other); 
    PoolInfo(MidiClient* seq);
    virtual ~PoolInfo();
    PoolInfo* clone();
    PoolInfo& operator=(const PoolInfo& other);
    int getSizeOfInfo() const;
    
    int getClientId();
    int getInputFree();
    int getInputPool();
    int getOutputFree();
    int getOutputPool();
    int getOutputRoom();
    void setInputPool(int size);
    void setOutputPool(int size);
    void setOutputRoom(int size);

private:    
    snd_seq_client_pool_t* m_Info;
};

class SequencerEventHandler
{
public:
    virtual void handleSequencerEvent(SequencerEvent* ev) = 0;
};

class MidiClient : public QObject
{
    Q_OBJECT

private:    
    class SequencerInputThread: public QThread
    {
    public:
        SequencerInputThread(MidiClient *seq, int timeout) 
            : QThread(),
            m_MidiClient(seq),
            m_Wait(timeout),
            m_Stopped(false) {}
        virtual ~SequencerInputThread() {}
        virtual void run();
        bool stopped();
        void stop();
    private:
        MidiClient *m_MidiClient;
        int m_Wait;
        bool m_Stopped;
        QReadWriteLock m_mutex;
    };    

public:
    MidiClient( QObject* parent = 0 );
    virtual ~MidiClient();

    void open();
    void open(snd_config_t* conf);
    void close();
    void startSequencerInput();
    void stopSequencerInput();
    MidiPort* createPort();
    MidiQueue* createQueue(); 
    MidiQueue* createQueue(QString const& name);
    MidiQueue* getQueue();
    MidiQueue* useQueue(int queue_id);
    MidiQueue* useQueue(const QString& name);
    MidiQueue* useQueue(MidiQueue* queue);
    void portAttach(MidiPort* port);
    void portDetach(MidiPort* port);
    void detachAllPorts();
    void addEventFilter(int evtype);
    void output(SequencerEvent* ev, bool async = false, int timeout = -1);
    void outputDirect(SequencerEvent* ev, bool async = false, int timeout = -1);
    void outputBuffer(SequencerEvent* ev, bool async = false, int timeout = -1);
    void drainOutput(bool async = false, int timeout = -1); 
    void synchronizeOutput();

    int getClientId();
    snd_seq_type_t getSequencerType();
    snd_seq_t* getHandle() { return m_SeqHandle; }
    bool isOpened() { return (m_SeqHandle != NULL); }

    size_t getOutputBufferSize();
    void setOutputBufferSize(size_t newSize);
    size_t getInputBufferSize();
    void setInputBufferSize(size_t newSize);
    QString getDeviceName() { return m_DeviceName; }
    void setDeviceName( QString const& newName);
    int getOpenMode() { return m_OpenMode; }
    void setOpenMode(int newMode);
    bool getBlockMode() { return m_BlockMode; }
    void setBlockMode(bool newValue);
    QString getClientName();
    void setClientName(QString const& newName);
    bool getBroadcastFilter();
    void setBroadcastFilter(bool newValue);
    bool getErrorBounce();
    void setErrorBounce(bool newValue);

    ClientInfo& getThisClientInfo();
    void setThisClientInfo(const ClientInfo& val);
    MidiPortList getMidiPorts() const;
    ClientInfoList getAvailableClients();
    PortInfoList getAvailableInputs();
    PortInfoList getAvailableOutputs();
    SystemInfo& getSystemInfo();
    QList<int> getAvailableQueues();
    
    PoolInfo& getPoolInfo();
    void setPoolInfo(const PoolInfo& info);
    void setPoolInput(int size);
    void setPoolOutput(int size);
    void setPoolOutputRoom(int size);
    void resetPoolInput();
    void resetPoolOutput();
    void dropInput();
    void dropInputBuffer();
    void dropOutput();
    void dropOutputBuffer();
    void removeEvents(const RemoveEvents* spec);
    SequencerEvent* extractOutput();
    int outputPending();
    int inputPending(bool fetch);
    int getQueueId(const QString& name);

    void addListener(QObject* listener);
    void removeListener(QObject* listener);
    void setEventsEnabled(const bool bEnabled);
    bool getEventsEnabled() const { return m_eventsEnabled; }
    void setHandler(SequencerEventHandler* handler)  { m_handler = handler; }

signals:
    void eventReceived(SequencerEvent* ev);

protected:
    void doEvents();
    void applyClientInfo();
    void readClients();
    void freeClients();
    void updateAvailablePorts();
    PortInfoList filterPorts(unsigned int filter);

    /* low level public functions */
    const char * _getDeviceName();
    int getPollDescriptorsCount(short events);
    int pollDescriptors(struct pollfd *pfds, unsigned int space, short events);
    unsigned short pollDescriptorsRevents(struct pollfd *pfds, unsigned int nfds);
    
    /* mid level functions */
    void _setClientName( const char *name );
    int createSimplePort( const char *name,
                          unsigned int caps,
                          unsigned int type );  
    void deleteSimplePort( int port );
    void connectFrom(int myport, int client, int port);
    void connectTo(int myport, int client, int port);
    void disconnectFrom(int myport, int client, int port);
    void disconnectTo(int myport, int client, int port);

private:
    bool m_eventsEnabled;
    bool m_BlockMode;
    bool m_NeedRefreshClientList;
    int  m_OpenMode;
    QString m_DeviceName;
    snd_seq_t* m_SeqHandle;
    QPointer<SequencerInputThread> m_Thread;
    QPointer<MidiQueue> m_Queue;
    SequencerEventHandler* m_handler;

    ClientInfo m_Info;
    ClientInfoList m_ClientList;
    MidiPortList m_Ports;
    PortInfoList m_OutputsAvail;
    PortInfoList m_InputsAvail;
    QObjectList m_listeners;
    SystemInfo m_sysInfo;
    PoolInfo m_poolInfo;
};

}
}

#endif //INCLUDED_CLIENT_H