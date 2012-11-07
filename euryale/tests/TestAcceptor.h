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
 * File:   TestAcceptor.h
 * Author: rmartins
 *
 * Created on April 19, 2010, 5:26 PM
 */

#ifndef _TESTACCEPTOR_H
#define	_TESTACCEPTOR_H

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


#include <hades/qos/net/packet/HadesPacket.h>
#include <hades/qos/net/packet/HadesPacketFactory.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>

class ASvc : public PacketDatagramChannel<HadesPacket, HadesHeader, HadesPacketFactory,
ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH> {
public:

    ASvc(bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<ACE_SOCK_Dgram_Mcast,ACE_MT_SYNCH>* closeListener=0) :
    PacketDatagramChannel<HadesPacket, HadesHeader, HadesPacketFactory,
    ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>(asyncRead, asyncWrite, inputQueue,
    ec, reserve, qos,closeListener) {
    }

    ~ASvc() {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=======>~ASvc\n")));
    }

    

    virtual int handlePacket(HadesPacket* packet) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)====>We have got a packet!\n")));
        delete packet;
        return 0;
    }
};


#endif	/* _TESTACCEPTOR_H */

