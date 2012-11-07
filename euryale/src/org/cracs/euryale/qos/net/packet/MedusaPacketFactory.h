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
 * File:   MedusaPacketFactory.h
 * Author: rmartins
 *
 * Created on December 18, 2009, 4:06 PM
 */

#ifndef _HADESPACKETFACTORY_H
#define	_HADESPACKETFACTORY_H

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/qos/net/packet/ThreadReserveReplyPacket.h>
#include <euryale/qos/net/packet/ThreadReserveRequestPacket.h>
#include <euryale/qos/net/packet/CPUGroupReserveReplyPacket.h>
#include <euryale/qos/net/packet/CPUGroupReserveRequestPacket.h>
#include <euryale/qos/net/packet/GetPartitionRequestPacket.h>
#include <euryale/qos/net/packet/GetPartitionReplyPacket.h>
#include <euryale/qos/net/packet/InitMedusaClientPacket.h>
#include <euryale/common/io/InputStream.h>

class MedusaPacketFactory {
public:

    template <class Packet> static MedusaPacket* createPacket(MedusaHeader* header, InputStream& is)
    {
        Packet* packet = new Packet();
        packet->init(header, is);
        return packet;
    }

    static MedusaPacket* createPacket(MedusaHeader* header, InputStream& is)
    {
        switch (header->m_messageType) {
        case THREAD_RESERVE_REQUEST_PACKET_ID:
        {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaPacketFactory: ThreadReserveRequestPacket\n")));
            return createPacket<ThreadReserveRequestPacket > (header, is);
        }
        case THREAD_RESERVE_REPLY_PACKET_ID:
        {
            return createPacket<ThreadReserveReplyPacket > (header, is);
        }
        case CPUGROUP_RESERVE_REQUEST_PACKET_ID:
        {
            return createPacket<CPUGroupReserveRequestPacket > (header, is);
        }
        case CPUGROUP_RESERVE_REPLY_PACKET_ID:
        {
            return createPacket<CPUGroupReserveReplyPacket > (header, is);
        }
        case GET_PARTITION_REQUEST_PACKET_ID:
        {
            return createPacket<GetPartitionRequestPacket > (header, is);
        }
        case GET_PARTITION_REPLY_PACKET_ID:
        {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaPacketFactory: GetPartitionReplyPacket\n")));
            return createPacket<GetPartitionReplyPacket > (header, is);
        }
        case CLIENT_INIT_ID:
        {
            return createPacket<InitMedusaClientPacket > (header, is);
        }
        default:
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaPacketFactory:No packet is known...\n")));
            return 0;
        }
    }


};

#endif	/* _HADESPACKETFACTORY_H */

