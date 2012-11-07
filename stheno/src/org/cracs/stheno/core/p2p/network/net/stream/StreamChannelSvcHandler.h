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
 * File:   StreamChannelSvcHandler.h
 * Author: rmartins
 *
 * Created on October 27, 2010, 11:12 AM
 */

#ifndef STREAMCHANNELSVCHANDLER_H
#define	STREAMCHANNELSVCHANDLER_H

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
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/network/net/stream/StreamChannel.h>

class StreamChannelSvcHandler : public StreamChannel {
public:
    typedef StreamChannel SuperType;
    typedef SuperType::CloseListenerType CloseListenerType;

    static const int MAX_OPEN_TIMEOUT_MS = 2000;
    
    
    StreamChannelSvcHandler(
            StreamChannelPacketFactory* factory,
            NetReservation* reserve = 0,
            NetQoS* qos = 0);
    
    /*StreamChannelSvcHandler(
            StreamChannelPacketFactory* factory,
            PacketHandler* readHandler,
            AsyncPacketWriterHandler* writeHandler,   
            bool asyncRead,bool asyncWrite,
            ExecutionModelPtr* ec,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0);*/
    
    StreamChannelSvcHandler(
            StreamChannelPacketFactory* factory,
            PacketHandler* readHandler,
            bool asyncWrite,
            ExecutionModelPtr* ec,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0);   
    virtual ~StreamChannelSvcHandler();

    virtual int open(void *arg = 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: StreamChannelSvcHandler::open\n")));
        int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: StreamChannelSvcHandler::open failed\n")));
            return -1;
        }
        //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
        ACE_Time_Value receiveTimeout;
        receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

        SthenoStreamPacket* recvPacket = 0;
        receivePacket(recvPacket, &receiveTimeout);
        if (recvPacket == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelSvcHandler::open - no init packet\n")));
            return -1;
        }
        switch (recvPacket->getPacketHeader()->getType()) {
            case InitSessionPacket::INIT_SESSION_PACKET_OP:
            {//from children
                InitSessionPacket* initSessionPacket = static_cast<InitSessionPacket*> (recvPacket);
                m_pid = initSessionPacket->getUUID();
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamChannelSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
                m_cellID = initSessionPacket->getFID();
                m_qosE = initSessionPacket->getQoS();
                return 0;
            }

            default:
            {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamChannelSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
                close();
                return -1;
            }
        }
    }

    UUIDPtr& getPID() {
        return m_pid;
    }

    UUIDPtr& getFID() {
        return m_cellID;
    }

    QoSEndpoint& getQoSEndpoint() {
        return m_qosE;
    }

protected:
    UUIDPtr m_pid;
    UUIDPtr m_cellID;
    QoSEndpoint m_qosE;

};


#endif	/* STREAMCHANNELSVCHANDLER_H */

