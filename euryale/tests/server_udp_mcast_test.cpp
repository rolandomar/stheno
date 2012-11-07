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
#include <euryale/net/PacketChannel.h>
#include <ace/Synch_Traits.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Dgram_Mcast.h>

#include <hades/qos/net/packet/HadesPacket.h>
#include <hades/qos/net/packet/HadesPacketFactory.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>

#include "TestAcceptor.h"

int main() {
    //ThreadPerConnection tpc;
    //A a(true,false,false,&tpc,0,0);
    bool async = true;
    bool queue = true;
    if (async) {
        ASvc server(true, true,queue);
        ACE_INET_Addr addr(7000, "224.1.1.1"); //"192.168.10.1");
        server.reactor(ACE_Reactor::instance());
        server.open(addr, "lo", 1);
        //ACE_Reactor::instance()->run_event_loop();
        ACE_Reactor::instance()->handle_events(0);
        HadesPacket* packet = 0;
        server.receivePacket(packet,0);
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Packet=%@\n",packet)));
    } else {
        ASvc server(false, false);
        HadesPacket* packet = 0;
        ACE_INET_Addr addr(7000, "224.1.1.1"); //"192.168.10.1");
        //server.reactor(ACE_Reactor::instance());
        server.open(addr, "lo", 1);
        server.receivePacket(packet, 0);
        //ACE_Reactor::instance()->run_event_loop();
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Here\n")));
    getchar();
    return 0;
}

