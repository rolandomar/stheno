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
 * File:   SthenoMulticastChannel.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 25, 2010, 4:04 PM
 */

#ifndef STHENOMULTICASTCHANNEL_H
#define	STHENOMULTICASTCHANNEL_H

#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
//#include "ace/LSOCK_Acceptor.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>
#include <euryale/net/PacketDatagramChannel.h>

#include <ace/Synch_Traits.h>

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/net/SthenoPacketFactory.h>

template <class PACKET_FACTORY>
class SthenoMulticastChannel : 
public PacketDatagramChannel<SthenoPacket, SthenoHeader, PACKET_FACTORY,
ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH> {
public:

    SthenoMulticastChannel(bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<ACE_SOCK_Dgram_Mcast,ACE_MT_SYNCH>* closeListener=0) :
    PacketDatagramChannel<SthenoPacket, SthenoHeader, PACKET_FACTORY,
ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>(asyncRead, asyncWrite, inputQueue,
    ec, reserve, qos,closeListener) {
       
    }

    virtual int handlePacket(SthenoPacket* packet) = 0;

    virtual ~SthenoMulticastChannel(){}
protected:


};

#endif	/* STHENOMULTICASTCHANNEL_H */

