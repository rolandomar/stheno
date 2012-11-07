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
 * File:   client_udp_mcast.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 25, 2010, 11:07:45 AM
 */


#include <euryale/net/PacketDatagramChannel.h>
#include <hades/qos/net/packet/HadesPacket.h>
#include <hades/qos/exception/CgroupException.h>
#include <hades/qos/cpu_qos/CPUQoS.h>
#include <hades/qos/cpu_qos/CPUPartition.h>
#include <hades/qos/net/packet/ThreadReserveRequestPacket.h>
#include <hades/qos/net/packet/ThreadReserveReplyPacket.h>
#include <hades/qos/net/packet/GetPartitionRequestPacket.h>
#include <hades/qos/net/packet/GetPartitionReplyPacket.h>

#include <hades/qos/net/packet/CPUGroupReserveRequestPacket.h>
#include <hades/qos/net/packet/CPUGroupReserveReplyPacket.h>
#include <hades/common/io/ByteInputStream.h>
#include <hades/common/io/ByteOutputStream.h>
#include <hades/qos/net/packet/HadesPacketFactory.h>
#include <ace/SOCK_Dgram_Mcast.h>

class ClientUDP : public PacketDatagramChannel<HadesPacket, HadesHeader, HadesPacketFactory,
ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH> {
public:

    ClientUDP(bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>* closeListener = 0) :
    PacketDatagramChannel<HadesPacket, HadesHeader, HadesPacketFactory,
    ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>(asyncRead, asyncWrite, inputQueue,
    ec, reserve, qos, closeListener) {
    }

    ~ClientUDP() {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=======ClientUDP\n"));
    }

    virtual int handlePacket(HadesPacket* packet) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)====>We have got a packet!\n"));
        delete packet;
        return 0;
    }

    virtual list<CPUPartition*>* getCPUPartitionList() throw (CgroupException*) {
        UInt id = 0;
        GetPartitionRequestPacket* packet = new GetPartitionRequestPacket(id);
        ACE_Time_Value* timeout = 0;
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)getCPUPartitionList()"));
        if (this->send(packet, timeout) != 0) {
            return 0;
        }
        return 0;
    }
};

int main() {
    ClientUDP* client = new ClientUDP(false, false);
    ACE_INET_Addr addr(7000,"224.1.1.1");
    client->open(addr,"lo",1);
    //client->reactor(ACE_Reactor::instance());
    //client->open(addr);
    client->getCPUPartitionList();
    //ACE_Reactor::instance()->run_event_loop();
    getchar();

    return (EXIT_SUCCESS);
}

