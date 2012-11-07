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
 * File:   StreamSvcHandler.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:48 PM
 */

#ifndef STREAMSERVICESVCHANDLER_H
#define	STREAMSERVICESVCHANDLER_H

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Reactor.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Stream.h"
#include "ace/LSOCK_Connector.h"
#include "ace/Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/net/PacketChannel.h>
#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/net/packet/InitSessionPacket.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>

#include <stheno/services/stream/net/packet/StreamServicePacketFactory.h>

class StreamService;

class StreamServiceSvcHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ACE_Strong_Bound_Ptr<StreamServiceSvcHandler,
    ACE_Recursive_Thread_Mutex> channel_ptr;

    static const int MAX_OPEN_TIMEOUT_MS = 2000;

    StreamServiceSvcHandler(StreamService* streamServer,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    virtual ~StreamServiceSvcHandler();

    virtual int open(void *arg = 0);

    virtual int handlePacket(SthenoStreamPacket* packet);

    UUIDPtr& getPID() {
        return m_pid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    int getFrameSize() {
        return m_frameSize;
    }

    int getFrameRate() {
        return m_frameRate;
    }

    UInt getLastFrameNo(){
        return m_lastFrameNo;
    }

protected:
    StreamService* m_streamServer;
    UInt m_lastFrameNo;
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    int m_frameSize;
    int m_frameRate;
};


#endif	/* STREAMSERVICESVCHANDLER_H */

