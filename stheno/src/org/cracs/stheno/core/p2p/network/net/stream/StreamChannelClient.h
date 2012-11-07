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
 * File:   P3DiscoveryClientHandler.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 12:21 PM
 */

#ifndef P3DISCOVERYCLIENTHANDLER_H
#define	P3DISCOVERYCLIENTHANDLER_H

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


#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>
#include <ace/Synch_Traits.h>

#include <euryale/net/PacketChannel.h>
#include <stheno/core/p2p/network/net/stream/StreamChannel.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>

/*switch (packet->getPacketHeader()->getType()) {

            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        
        return 0;*/

class StreamChannelClient : public StreamChannel {
public:

    typedef StreamChannel SuperType;
    typedef SuperType::CloseListenerType CloseListenerType;

    StreamChannelClient();

    StreamChannelClient(
            StreamChannelPacketFactory* factory,
            UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qosE,
            NetReservation* reserve = 0,
            NetQoS* qos = 0);

    StreamChannelClient(
            StreamChannelPacketFactory* factory,
            UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qosE,
            PacketHandler* readHandler,
            bool asyncWrite,
            ExecutionModelPtr* ec,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0);

    virtual ~StreamChannelClient();

    int open(void *arg = 0) {
        InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_channelQoS);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
        if (ret == -1) {
            return -1;
        }
        ret = StreamChannel::open(arg);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
        return ret;
    }

    static const int JOIN_CELL_TIMEOUT_MS = 20000;


};


#endif	/* P3DISCOVERYCLIENTHANDLER_H */

