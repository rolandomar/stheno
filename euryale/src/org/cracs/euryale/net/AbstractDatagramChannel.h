/*
 *  Copyright 2012 Rolando Martins, CRACS & INESC-TEC, DCC/FCUP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *   
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 */
/* 
 * File:   AbstractChannel.h
 * Author: rmartins
 *
 * Created on December 4, 2009, 11:29 AM
 */

#ifndef _ABSTRACTDATAGRAMCHANNEL_H
#define	_ABSTRACTDATAGRAMCHANNEL_H

#include <ace/Message_Block.h>
#include <ace/Message_Queue.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor_Notification_Strategy.h>
//for template spec.
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Mcast.h>

#include <euryale/qos/net_qos/NetReservation.h>
#include <euryale/qos/net_qos/NetQoS.h>
//event
#include <euryale/event/Event.h>
//threading
#include <euryale/threading/ExecutionModel.h>
//net
#include <euryale/net/EventHandler.h>
#include <euryale/net/WriteDatagramBlock.h>
#include <euryale/net/CloseDatagramListener.h>

//#include <euryale/net/packet/Packet.h>
//#include <stdio.h>
//#include <euryale/net/AbstractStreamChannel.h>

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class AbstractDatagramChannel : public EventHandler {
public:
    typedef ACE_PEER_STREAM_ADDR addr_type;
    typedef ACE_PEER_STREAM stream_type;
    typedef AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE> channel_type;
    typedef CloseDatagramListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE> close_listener;

    AbstractDatagramChannel();

    AbstractDatagramChannel(bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            close_listener* closeListener = 0);

    /*AbstractDatagramChannel(NetReservation* reserve = 0,
            NetQoS* qos = 0);*/

    virtual ~AbstractDatagramChannel();

    virtual int close(u_long flags = 0);

    //Multicast
    virtual int open(const ACE_INET_Addr &mcast_addr, // Bound & sendto address.
            const ACE_TCHAR *net_if, // Net interface for sends.
            int reuse_addr = 1);

    //Point-to-Point UDP
    virtual int open(const ACE_INET_Addr &local,
            int protocol_family = ACE_PROTOCOL_FAMILY_INET,
            int protocol = 0,
            int reuse_addr = 0);

    virtual int synchronous();
    virtual int asynchronous(bool readAsync, bool writeAsync);

    //Synchronous send

    virtual int send(ACE_Message_Block* mb, ACE_INET_Addr* addr = 0, ACE_Time_Value* timeout = 0);

    //virtual int sendSync(ACE_Message_Block* mb, ACE_Time_Value* timeout);

    virtual int sendSync(ACE_Message_Block* mb, ACE_INET_Addr* addr, ACE_Time_Value* timeout);

    virtual int sendAsync(WriteDatagramBlockPtr* wb = 0);

    virtual int receiveSync(ACE_Message_Block* mb, int recv, ACE_INET_Addr* addr, ACE_Time_Value* timeout);
    //Synchronous receive

    virtual int receive(ACE_Message_Block*& mb, int recv, ACE_INET_Addr* addr = 0, ACE_Time_Value* timeout = 0);

    bool hasAsyncWrite();

    bool hasAsyncRead();

    ACE_PEER_STREAM_2& peer();

    virtual ACE_HANDLE get_handle(void) const;

    ExecutionModel* getExecutionModel();

protected:
    bool m_closed;
    ExecutionModelPtr* m_ec; //release: n/a
    NetReservation* m_reserve; ////release: this
    NetQoS* m_qos; //release: thiss
    ACE_PEER_STREAM_2 m_peer;
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_asyncWrite;
    bool m_asyncRead;
    ACE_Message_Queue<ACE_SYNCH_USE> m_inputQueue;
    ACE_Message_Queue<ACE_SYNCH_USE> m_outputQueue;
    bool m_hasInputQueue;
    ACE_Reactor_Notification_Strategy m_notifier;
    CloseDatagramListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* m_closeListener;
    ACE_INET_Addr* m_mcast;

    virtual int handleClose_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handleInputQueue_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handleOutput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int queueOutput(WriteDatagramBlockPtr*& wb);

};

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::AbstractDatagramChannel() :
m_ec(0), m_reserve(0), m_qos(0),
m_asyncWrite(false), m_asyncRead(false),
m_hasInputQueue(false), m_notifier(0, this, ACE_Event_Handler::WRITE_MASK),
m_closeListener(0), m_mcast(0), m_closed(false) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractDatagramChannel:AbstractDatagramChannel(1) %@\n"), this));
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::AbstractDatagramChannel(bool asyncRead, bool asyncWrite,
        bool inputQueue,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        close_listener* closeListener) :
m_ec(ec), m_reserve(reserve), m_qos(qos),
m_asyncWrite(asyncWrite), m_asyncRead(asyncRead),
m_hasInputQueue(inputQueue), m_notifier(0, this, ACE_Event_Handler::WRITE_MASK),
m_closeListener(closeListener), m_mcast(0), m_closed(false) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractDatagramChannel:AbstractDatagramChannel(2) %@\n"), this));
    //check if either m_asyncWrite || m_asyncRead then we must have
    //a EC
    //if inputQueue == true must have asyncRead == true

}

/*template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::AbstractDatagramChannel(NetReservation* reserve,
        NetQoS* qos) :
m_ec(0), m_reserve(reserve), m_qos(qos),
m_asyncWrite(false), m_asyncRead(false),
m_hasInputQueue(false), m_closeListener(0), m_mcast(0) {
}*/

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::~AbstractDatagramChannel() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractDatagramChannel::~AbstractDatagramChannel() %@\n"), this));
    m_closeListener = 0;

    //m_closeListener = 0;
    if (m_ec != 0) {
        delete m_ec;
    }
    if (m_reserve != 0) {
        delete m_reserve;
    }
    if (m_qos != 0) {
        delete m_qos;
    }
    if (m_mcast != 0) {
        delete m_mcast;
    }

    close();
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::close(u_long flags) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return 0;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::close()\n")));
    m_closed = true;
    if (m_asyncWrite || m_asyncRead) {
        if (reactor() != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::close() - remove handler\n")));
            reactor()->remove_handler(this, ALL_EVENTS_MASK | DONT_CALL);
        }
    }
    m_peer.close();
    if (m_closeListener != 0) {
        ace_mon.release();
        m_closeListener->onClose(this);
    }

    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::synchronous() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    if (reactor() != 0) {
        //this->m_notifier.reactor(this->reactor());
        //if (m_asyncWrite) {
        this->m_notifier.reactor(0);
        m_outputQueue.notification_strategy(0);
        //this->m_outputQueue.notification_strategy(&m_notifier);
        //}
        //int ret = this->reactor()->remove_handler(this, ACE_Event_Handler::READ_MASK);
        //int ret = this->reactor()->suspend_handler(this);
        int ret = this->reactor()->remove_handler(this, ALL_EVENTS_MASK | DONT_CALL);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)AbstractDatagramChannel::synchronous() RET=%d\n", ret);
    }
    m_asyncWrite = false;
    m_asyncRead = false;
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::asynchronous(bool readAsync, bool writeAsync) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO AbstractDatagramChannel::asynchronous() already closed!\n")));
        return -1;
    }
    if (reactor() != 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)asynchronous jsdfkldjsjflsdjfklj\n");
        m_asyncWrite = writeAsync;
        m_asyncRead = readAsync;
        if (m_asyncWrite) {
            this->m_notifier.reactor(this->reactor());
            this->m_outputQueue.notification_strategy(&m_notifier);
        }
        int ret = this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
        //int ret = this->reactor()->resume_handler(this);            
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)AbstractDatagramChannel::asynchronous() RET=%d\n"), ret));
        return ret;
    } else {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO AbstractDatagramChannel::asynchronous() no reactor!\n")));
    }
    return -1;
}

//Synchronous send

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::send(ACE_Message_Block* mb, ACE_INET_Addr* addr,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    if (hasAsyncWrite()) {
        //async socket, sync send
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)send()\n");
        WriteDatagramBlockPtr* wb = new WriteDatagramBlockPtr(new WriteDatagramBlock(mb->duplicate(), addr, timeout));
        WriteDatagramBlockPtr* wb2 = new WriteDatagramBlockPtr(*wb);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - AsyncWrite(%l) before sendAsync\n"), mb->length()));
        int ret = sendAsync(wb2);
        if (ret != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: AbstractDatagramChannel::send() - sendAsync failed -1\n")));
            delete wb;
            ace_mon.release();
            close();
            return -1;
        }
        if (m_ec != 0) {
            try {
                //ace_mon.release();
                //(*m_ec)->join(wb, wb->getTimeout());
                ExecutionModelPtr threadContextEC(*m_ec);
                ace_mon.release();
                threadContextEC->join(wb->get(), (*wb)->getTimeout());
            } catch (ExecutionModelException& ex) {
                (*wb)->wait(timeout);
            }
            if ((*wb)->successful()) {
                delete wb;
                return 0;
            } else {
                delete wb;                
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)Error: AbstractDatagramChannel::send() - !wb->successful()!!\n")));
                ace_mon.release();
                close();
                return -1;
                //return 0;
            }
        } else {
            delete wb;
            return -1;//0;
        }
        return 0;
    } else {
        //sync socket, sync send

        if (timeout != 0) {
            ACE_Time_Value now = ACE_OS::gettimeofday();
            if (now > *timeout) {
                ace_mon.release();
                close();
                return -1;
            }
        }
        int ret = sendSync(mb, addr, timeout);
        if (ret == -1) {
            return -1;
        }


        return 0;
    }
}

/*template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::sendSync(ACE_Message_Block* mb, ACE_Time_Value* timeout) {
    return sendSync(mb, 0, timeout);
}*/

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::sendSync(ACE_Message_Block* mb, ACE_INET_Addr* addr, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    int flags = 0;
    if (addr == 0) {
        if (m_mcast != 0) {
            addr = m_mcast;
        }
    }
    int ret = ((ACE_SOCK_Dgram) peer()).send(mb->rd_ptr(), mb->length(), *addr, flags, timeout);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::send(%s) - ret=%d\n"), addr->get_host_addr(), ret));
    if (ret == -1) {
        ace_mon.release();
        close();
        return -1;
    }
    mb->rd_ptr(ret);
    return ret;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::sendAsync(WriteDatagramBlockPtr* wb) {
    //int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::sendAsync(WriteDatagramBlock* wb) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    int queueRet = queueOutput(wb);
    if (queueRet == -1) {
        ace_mon.release();
        close();
        return -1;
    }
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::
receiveSync(ACE_Message_Block* mb, int recv, ACE_INET_Addr* addr, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    int flags = 0;
    if (addr == 0) {
        if (m_mcast != 0) {
            addr = m_mcast;
        }
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::recv()\n")));
    int ret = peer().recv(mb->wr_ptr(), recv, *addr, flags, timeout);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::recv() - ret=%d\n"), ret));
    if (ret == 0) {
        //timeout
        return 0;
    }
    if (ret == -1) {
        ace_mon.release();
        close();
        return -1;
    }
    mb->wr_ptr(ret);
    return ret;
}

//Synchronous receive

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int
AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::
receive(ACE_Message_Block*& mb, int recv, ACE_INET_Addr* addr, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_closed) {
        return -1;
    }
    if (hasAsyncRead()) {
        //async socket, sync read
        if (m_hasInputQueue) {
            return m_inputQueue.dequeue(mb, timeout);
        }
        return -1;
    } else {
        while (recv > 0) {
            if (timeout != 0) {
                ACE_Time_Value now = ACE_OS::gettimeofday();
                if (now > *timeout) {
                    ace_mon.release();
                    close();
                    return -1;
                }
            }
            int ret = receiveSync(mb, recv, addr, timeout);
            if (ret == -1) {
                return -1;
            }
            recv -= ret;
        }
    }
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
bool AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::hasAsyncWrite() {
    return m_asyncWrite;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
bool AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::hasAsyncRead() {
    return m_asyncRead;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
ACE_PEER_STREAM_2& AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::peer() {
    return m_peer;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
ACE_HANDLE AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::get_handle(void) const {
    return this->m_peer.get_handle();
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
ExecutionModel* AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::getExecutionModel() {
    return m_ec;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::handleClose_i(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: AbstractDatagramChannel::handleClose_i()\n")));
    /*if (m_closeListener != 0) {
        m_closeListener->onClose(this);
    }
    delete this;*/
    close();
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::handleInput_i(ACE_HANDLE fd) {
    // ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleInput_i\n");
    if (m_hasInputQueue) {
        return handleInputQueue_i(fd);
    }
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::handleOutput_i(ACE_HANDLE fd) {
    ACE_Message_Block* mbMeta = 0;
    ACE_Time_Value now = ACE_OS::gettimeofday();
    // ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) m_outputQueue=%lu\n", m_outputQueue.message_count());
    int ret = m_outputQueue.dequeue(mbMeta, &now);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: AbstractDatagramChannel: handleOutput_i() -enqueue failure!\n")));
        //reactor()->remove_handler(this, ACE_Event_Handler::WRITE_MASK);
        return -1;
    }
    if (mbMeta == 0) {
        return 0;
    }
    //WriteBlock* wb = (WriteBlock*)mbMeta->base();
    WriteDatagramBlockPtr* wb = 0;
    ACE_OS::memcpy(&wb, mbMeta->base(), sizeof (WriteDatagramBlockPtr*));
    ret = sendSync((*wb)->getMessageBlock(), (*wb)->getAddr(), &now);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) m_outputQueue=%lu RET====%d\n", m_outputQueue.message_count(), ret);
    if (ret == -1) {
        ACE_Message_Block::release(mbMeta);
        delete wb;
        return -1;
    }
    if ((*wb)->getMessageBlock()->length() > 0) {
        ACE_Message_Block::release(mbMeta);
        delete wb;
        return -1;
        //return m_outputQueue.enqueue_head(mbMeta, &now);
    } else {
        (*wb)->stateChanged(Event::ES_SUCCESS);
        ACE_Message_Block::release(mbMeta);
        delete wb;
    }
    return 0;
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
//int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::queueOutput(WriteDatagramBlock*& wb) {
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::queueOutput(WriteDatagramBlockPtr*& wb) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)queueOutput\n");
    ACE_Message_Block* mbMeta = new ACE_Message_Block(sizeof (WriteDatagramBlockPtr*));
    ACE_OS::memcpy(mbMeta->base(), &wb, sizeof (WriteDatagramBlockPtr*));
    ACE_Time_Value now = ACE_OS::gettimeofday();
    return m_outputQueue.enqueue(mbMeta, &now);
}

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
int AbstractDatagramChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>::handleInputQueue_i(ACE_HANDLE fd) {
    ACE_Message_Block* mb = new ACE_Message_Block(1024);
    ACE_Time_Value now = ACE_OS::gettimeofday();
    ACE_INET_Addr addr;
    int ret = receiveSync(mb, mb->size(), &addr, &now);
    if (ret == -1) {
        return -1;
    }
    if (ret == 0) {
        return 0;
    }
    return m_inputQueue.enqueue_tail(mb, &now);
}


#endif	/* _ABSTRACTCHANNEL_H */

