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

#ifndef _ABSTRACTCHANNEL_H
#define	_ABSTRACTCHANNEL_H
#include <ace/os_include/netinet/os_tcp.h>
#include <ace/Message_Block.h>
#include <ace/Message_Queue.h>
#include <ace/Signal.h>
#include <euryale/qos/net_qos/NetReservation.h>
#include <euryale/qos/net_qos/NetQoS.h>
#include <euryale/net/EventHandler.h>
#include <euryale/event/Event.h>
#include <euryale/threading/ExecutionModel.h>
#include <euryale/net/packet/Packet.h>

#include <ace/Reactor_Notification_Strategy.h>

#include <euryale/common/Backtrace.h>

#include <euryale/net/WriteBlock.h>
#include <euryale/net/CloseListener.h>
#include <euryale/runtime/RuntimeConfiguration.h>


//template <class SOCK_TYPE, class SYNC_TYPE>

template <ACE_PEER_STREAM_1, ACE_SYNCH_DECL>
class AbstractStreamChannel : public EventHandler {
public:
    typedef ACE_PEER_STREAM_ADDR addr_type;
    typedef ACE_PEER_STREAM stream_type;
    typedef AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE> channel_type;
    typedef CloseListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE> CloseListenerType;
    typedef ACE_Strong_Bound_Ptr<AbstractStreamChannel<ACE_PEER_STREAM_2, ACE_SYNCH_USE>,
    ACE_Recursive_Thread_Mutex> channel_ptr;
    bool m_debugAbstractStreamChannel;

    static const int SMALL_WRITE_THRESHOLD = 512;
    static const int SMALL_WRITE_OPTIMUM_VALUE = 56;

    AbstractStreamChannel() : m_ec(0), m_reserve(0), m_qos(0),
    m_asyncWrite(false), m_asyncRead(false),
    /*m_hasInputQueue(false),*/ m_notifier(0, this, ACE_Event_Handler::WRITE_MASK),
    m_closeListener(0), m_close(false) {
        m_debugAbstractStreamChannel = RuntimeConfiguration::isClassDebugged("AbstractStreamChannel");
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel:AbstractStreamChannel(1) %@\n"), this));
        }

        UUID* channelID = UUID::generateUUID();
        m_channelID.reset(channelID);
    }

    /**
     * Document AbstractChannel(ChannelInputListener<class,class>*,
     * ChannelInputListener<class,class>*,NetReservation*,NetQoS*) here...
     *
     * @param inputListener
     * @param outputListener
     * @param reserve
     * @param qos
     * @author Rolando Martins (rolando.martins@gmail.com)
     */
    AbstractStreamChannel(bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0) :
    m_ec(ec), m_reserve(reserve), m_qos(qos),
    m_asyncWrite(asyncWrite), m_asyncRead(asyncRead),
    /*m_hasInputQueue(inputQueue), */m_notifier(0, this, ACE_Event_Handler::WRITE_MASK),
    m_closeListener(closeListener), m_close(false) {
        m_debugAbstractStreamChannel = RuntimeConfiguration::isClassDebugged("AbstractStreamChannel");
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel:AbstractStreamChannel(2) %@\n"), this));
        }
        UUID* channelID = UUID::generateUUID();
        m_channelID.reset(channelID);
        //check if either m_asyncWrite || m_asyncRead then we must have
        //a EC
        //if inputQueue == true must have asyncRead == true

    }

    /**
     * Document AbstractChannel(NetReservation*,NetQoS*) here...
     *
     * @param reserve
     * @param qos
     * @author Rolando Martins (rolando.martins@gmail.com)
     */
    AbstractStreamChannel(NetReservation* reserve,
            NetQoS* qos) :
    m_ec(0), m_reserve(reserve), m_qos(qos),
    m_asyncWrite(false), m_asyncRead(false),
    /*m_hasInputQueue(false), */m_closeListener(0), m_close(false) {
        m_debugAbstractStreamChannel = RuntimeConfiguration::isClassDebugged("AbstractStreamChannel");
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel:AbstractStreamChannel(3) %@\n"), this));
        }
        UUID* channelID = UUID::generateUUID();
        m_channelID.reset(channelID);
    }

    virtual UUIDPtr& getPID() = 0;

    virtual UUIDPtr& getChannelID() {
        return m_channelID;
    }

    virtual bool isValid() {
        return (!m_close);
    }

    virtual ~AbstractStreamChannel() {
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel::~AbstractChannel() this=%@\n"), this));
        }
        //Backtrace::backstrace();        
        if (m_ec != 0) {
            delete m_ec;
        }
        if (m_reserve != 0) {
            delete m_reserve;
        }
        if (m_qos != 0) {
            delete m_qos;
        }
        if (!m_close) {
            if (m_debugAbstractStreamChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::~AbstractStreamChannel() - calling close\n")));
            }
            m_closeListener = 0;
            close();
        }

    }

    virtual int open(void *arg = 0) {
        //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::open()\n")));
        }
        //this->reactor(m_ec->getReactor());
        int nodelay = 1; // Disable Nagle’s algorithm
        int ret = peer().set_option(ACE_IPPROTO_TCP,
                TCP_NODELAY,
                &nodelay,
                sizeof (nodelay));
        /*int recvBufferSize = 4096;//10240;
        int sendBufferSize = 4096;//10240;
        peer().set_option(SOL_SOCKET,
                TCP_NODELAY,
                &nodelay,
                sizeof (nodelay));
        
        ret = peer().set_option(SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, int(sizeof(int)));
        if(ret == -1){
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: failed to set send buffer size!!!\n")));
        }
        ret = peer().set_option(SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, int(sizeof(int)));
        if(ret == -1){
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: failed to set recv buffer size!!!\n")));
        }*/

        if (this->reactor() != 0) {
            if (m_asyncWrite) {
                this->m_notifier.reactor(this->reactor());
                this->m_outputQueue.notification_strategy(&m_notifier);
            }
            if (m_asyncRead) {
                int ret = this->reactor()->register_handler
                        (this, ACE_Event_Handler::READ_MASK);
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel::open() RET=%d\n"), ret));
                return ret;
            }
            return 0;

        } else {
            if (m_debugAbstractStreamChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Reactor is NULL!!!!!!\n")));
            }
            if (!m_asyncWrite && !m_asyncRead) {
                return 0;
            }
            return -1;
        }

        return 0;
    }

    virtual int setCloseListener(CloseListenerType* closeListener) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (m_closeListener != 0) {
            if (m_debugAbstractStreamChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: AbstractStreamChannel: setCloseListener() already has listener!\n")));
            }
            return -1;
        }
        if (m_close) {
            return -1;
        }
        m_closeListener = closeListener;
        return 0;
    }


    //Synchronous send

    virtual int send(ACE_Message_Block* mb, ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (m_close) {
            return -1;
        }
        if (hasAsyncWrite()) {
            //async socket, sync send
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - AsyncWrite(%d)\n"), mb->length()));
            WriteBlockPtr* wb = new WriteBlockPtr(new WriteBlock(mb->duplicate(), timeout));
            WriteBlockPtr* wb2 = new WriteBlockPtr(*wb);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - AsyncWrite(%l) before sendAsync\n"), mb->length()));
            int ret = sendAsync(wb2);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - after sendAsync\n")));
            if (ret != 0) {
                if (m_debugAbstractStreamChannel) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: AbstractStreamChannel::send() - ret != 0, calling close()\n")));
                }
                ace_mon.release();
                close();
                return -1;
            }
            try {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - before join\n")));
                ExecutionModelPtr threadContextEC(*m_ec);
                ace_mon.release();
                //TODO: or check for error condition
                //while (!(*wb)->successful()) {
                while (!(*wb)->successful() && !(*wb)->isStateFinal()) {
                    threadContextEC->join(wb->get(), (*wb)->getTimeout());
                }
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - after join\n")));
            } catch (ExecutionModelException& ex) {
                delete wb;
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - ex - wait for event\n")));
                if (ex.getType() != ExecutionModelException::JOIN_FAILED) {
                    ace_mon.release();
                    close();
                    return -1;
                }
                int ret = (*wb)->wait(timeout);

                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - ex - after wait for event, ret=\n"), ret));
                if (ret == -1) {
                    delete wb;
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - ex - after wait for event, ret=\n"), ret));
                    ace_mon.release();
                    close();
                    return -1;
                }
            }
            if ((*wb)->successful()) {
                delete wb;
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - ok\n")));
                return 0;
            } else {
                delete wb;
                //delete
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: AbstractStreamChannel::send() - !wb->successful(), calling close()!!\n")));
                ace_mon.release();
                close();
                return -1;
                //return 0;
            }
        } else {
            //sync socket, sync send
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractChannel:: send() - syncWrite(%d)\n"), mb->length()));
            while (mb->length() > 0) {
                if (timeout != 0) {
                    ACE_Time_Value now = ACE_OS::gettimeofday();
                    if (now > *timeout) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: AbstractChannel:: send() - syncWrite, timeout!, calling close()\n")));
                        ace_mon.release();
                        close();
                        return -1;
                    }
                }
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)AbstractChannel:: send() - sync - sending(%Q) timeout(%@)...\n"),mb->length(),timeout));
                //int ret = sendSync(mb, 0);//timeout);
                int ret = sendSync(mb, timeout);
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)AbstractChannel:: send() - sync - SENT(%d) left(%d)!\n"), ret, mb->length()));
                if (ret == -1) {
                    //if (m_debugAbstractStreamChannel) {
                       // ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: AbstractChannel:: send() - sync - sent failed,callsing close()\n"), ret));
                    //}
                    close(); //already closed                    
                    return -1;
                }
            }
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)AbstractChannel:: send() - sync - sent OK\n")));
            return 0;
        }
    }

    virtual int sendSync(ACE_Message_Block* mb, ACE_Time_Value* timeout) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        //ACE_Sig_Action no_sigpipe((ACE_SignalHandler) SIG_IGN);
        //ACE_Sig_Action original_action;
        //no_sigpipe.register_action(SIGPIPE, &original_action);
        size_t writeSize = mb->length();
        /*if (writeSize < SMALL_WRITE_THRESHOLD) {
            if (writeSize > SMALL_WRITE_OPTIMUM_VALUE) {
                writeSize = SMALL_WRITE_OPTIMUM_VALUE;
            }
        }*/
        //int ret = peer().send(mb->rd_ptr(), mb->length(), timeout);
        //printf("before send timeout=%p\n",timeout);
        int ret = peer().send(mb->rd_ptr(), writeSize, timeout);
        //no_sigpipe.restore_action(SIGPIPE, original_action);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::sendSync() - SNT=%d\n"),ret));
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::sendSync() - calling close writeSize=%d ret=%d errmno=%s\n"),
                    writeSize,ret,strerror(errno)));
            ace_mon.release();
            close();
            return -1;
        }
        mb->rd_ptr(ret);
        return ret;
    }

    //virtual int sendAsync(WriteBlock* wb = 0) {

    virtual int sendAsync(WriteBlockPtr* wb = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        int queueRet = queueOutput(wb);
        if (queueRet == -1) {
            if (m_debugAbstractStreamChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::sendASync() - calling close\n")));
            }
            ace_mon.release();
            close();
            return -1;
        }
        return 0;
    }

    virtual int receiveSync(ACE_Message_Block* mb, int recv, ACE_Time_Value* timeout) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        int ret = peer().recv(mb->wr_ptr(), recv, timeout);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::receiveSync() - RECV=%d\n"),ret));
        if (ret == 0) {
            //timeout
            return 0;
        }
        if (ret == -1) {
            //if (m_debugAbstractStreamChannel) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::receiveSync() - calling close\n")));
            //}
            ace_mon.release();
            close();
            return -1;
        }
        mb->wr_ptr(ret);
        return ret;
    }

    //Synchronous receive

    virtual int receive(ACE_Message_Block*& mb, int recv, ACE_Time_Value* timeout) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (hasAsyncRead()) {
            //async socket, sync read
            /*if (m_hasInputQueue) {
                return m_inputQueue.dequeue(mb, timeout);
            }*/
            return -1;
        } else {
            while (recv > 0) {
                if (timeout != 0) {
                    ACE_Time_Value now = ACE_OS::gettimeofday();
                    if (now > *timeout) {
                        if (m_debugAbstractStreamChannel) {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::receive() - timeout, calling close\n")));
                        }
                        ace_mon.release();
                        close();
                        return -1;
                    }
                }
                //int ret = receiveSync(mb, recv, 0/*timeout*/);
                int ret = receiveSync(mb, recv, timeout);
                if (ret == -1) {
                    //if (m_debugAbstractStreamChannel) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::receive() - -1\n")));
                    //}
                    return -1;
                }
                recv -= ret;
            }
        }
        return 0;
    }

    virtual int close(u_long flags = 0) {
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: AbstractStreamChannel::close() - before lock %@\n"), this));
        }
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (m_close) {
            if (m_debugAbstractStreamChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: AbstractStreamChannel::close() - already closed %@\n"), this));
            }
            return 0; //-1;
        }
        ace_mon.release();
        //if (m_debugAbstractStreamChannel) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: AbstractStreamChannel::close() %@\n"), this));
        //}
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::!close() this=%@ pid=%s\n"),this,getPID()->toString().c_str()));
        //Backtrace::backstrace();
        m_close = true;
        if (m_asyncWrite || m_asyncRead) {
            if (reactor() != 0) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() Remove handler\n")));
                reactor()->remove_handler(this, ALL_EVENTS_MASK | DONT_CALL);
            }
        }
        m_peer.close();
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close()  - Closing - flags=%d mask=%d dontcall=%d\n"),flags,(flags & DONT_CALL),DONT_CALL));
        if ((flags & DONT_CALL) == 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() fire onclose\n")));
            if (m_closeListener != 0) {
                //ace_mon.release();
                if (m_debugAbstractStreamChannel) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() calling listener->onClose() %@\n"), this));
                }
                m_closeListener->onClose(this);
                //m_closeListener = NULL;
                //if (m_debugAbstractStreamChannel) {
                //    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() after call listener->onClose() %@\n"), this));
                //}
            } else {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() no listener\n")));
            }
        } else {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() dont fire onclose\n")));
        }
        /*if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::close() done! %@\n"), this));
        }*/
        return 0;
    }

    virtual int handleClose_i(ACE_HANDLE handle,
            ACE_Reactor_Mask close_mask) {
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleClose_i() - calling close\n")));
        }
        close();
        return 0;
    }

    virtual int synchronous() {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
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

    virtual int asynchronous(bool readAsync, bool writeAsync) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::asynchronous(%d,%d) this=%@ this=%s\n"), readAsync, writeAsync, this, getPID()->toString().c_str()));
        if (reactor() != 0) {
            m_asyncWrite = writeAsync;
            m_asyncRead = readAsync;

            if (m_asyncWrite) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::asynchronous() - async write, activating write notifier\n")));
                this->m_notifier.reactor(this->reactor());
                this->m_outputQueue.notification_strategy(&m_notifier);
            } else {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::asynchronous() - no async write, disable notifier\n")));
                this->m_notifier.reactor(0);
                m_outputQueue.notification_strategy(0);
            }
            if (readAsync) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::asynchronous() - async read, adding read mask to reactor.\n")));
                return this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
            } else {
                if (m_asyncWrite) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::asynchronous() - no async read, clearing the read mask.\n")));
                    return this->reactor()->register_handler(this, ACE_Event_Handler::NULL_MASK);
                } else {
                    return 0;
                }
            }
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: AbstractStreamChannel::asynchronous() - No reactor!\n")));
            return -1;
        }
    }

    bool hasAsyncWrite() {
        return m_asyncWrite;
    }

    bool hasAsyncRead() {
        return m_asyncRead;
    }

    ACE_PEER_STREAM_2& peer() {
        return m_peer;
    }

    virtual ACE_HANDLE get_handle(void) const {
        return this->m_peer.get_handle();
    }

    ExecutionModel* getExecutionModel() {
        return m_ec;
    }

    ACE_Message_Queue<ACE_SYNCH_USE>& getOuputQueue() {
        return m_outputQueue;
    }

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    UUIDPtr m_channelID;
    ExecutionModelPtr* m_ec; //release: n/a
    NetReservation* m_reserve; ////release: this
    NetQoS* m_qos; //release: this
    ACE_PEER_STREAM_2 m_peer;
    //ACE_PEER_STREAM_2 m_lock;
    bool m_asyncWrite;
    bool m_asyncRead;
    //ACE_Message_Queue<ACE_SYNCH_USE> m_inputQueue;
    ACE_Message_Queue<ACE_SYNCH_USE> m_outputQueue;
    //bool m_hasInputQueue;
    ACE_Reactor_Notification_Strategy m_notifier;
    CloseListener<ACE_PEER_STREAM_2, ACE_SYNCH_USE>* m_closeListener;
    bool m_close;

    /*virtual int handleClose_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleClose_i()\n")));
        if (m_closeListener != 0) {
            m_closeListener->onClose(this);
        }
        delete this;
    }*/

    virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) = 0;

    /*virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleInput_i()\n")));
        if (m_hasInputQueue) {
            return handleInputQueue_i(fd);
        }
        return 0;
    }*/

    virtual int handleOutput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        if (m_debugAbstractStreamChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleOutput_i()\n")));
        }
        ACE_Message_Block* mbMeta = 0;
        ACE_Time_Value now = ACE_OS::gettimeofday();
        m_outputQueue.dequeue(mbMeta, &now);
        if (mbMeta == 0) {
            return 0;
        }
        //WriteBlock* wb = (WriteBlock*)mbMeta->base();
        WriteBlockPtr* wb = 0;
        ACE_OS::memcpy(&wb, mbMeta->base(), sizeof (WriteBlockPtr*));
        int ret = sendSync((*wb)->getMessageBlock(), &now);
        if (ret == -1) {
            ACE_Message_Block::release(mbMeta);
            delete wb;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleOutput_i() failed to send\n")));
            return -1;
        }
        if ((*wb)->getMessageBlock()->length() > 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: AbstractStreamChannel::handleOutput_i() enqueing again\n")));
            return m_outputQueue.enqueue_head(mbMeta, &now);
        } else {
            (*wb)->stateChanged(Event::ES_SUCCESS);
            ACE_Message_Block::release(mbMeta);
            delete wb;
        }
        return 0;
    }

    //int queueOutput(WriteBlock*& wb) {

    int queueOutput(WriteBlockPtr*& wb) {
        //ACE_Message_Block* mbMeta = new ACE_Message_Block(sizeof (WriteBlock*));
        //ACE_OS::memcpy(mbMeta->base(), &wb, sizeof (WriteBlock*));
        ACE_Message_Block* mbMeta = new ACE_Message_Block(sizeof (WriteBlockPtr*));
        ACE_OS::memcpy(mbMeta->base(), &wb, sizeof (WriteBlockPtr*));
        ACE_Time_Value now = ACE_OS::gettimeofday();
        return m_outputQueue.enqueue(mbMeta, &now);
    }
};
#include <ace/SOCK_Stream.h>

typedef AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::channel_ptr SockStreamChannelPtr;

#endif	/* _ABSTRACTCHANNEL_H */

