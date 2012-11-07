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
 * File:   RPCServicePacketFactory.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:17 PM
 */

#include "RPCServicePacketFactory.h"

SthenoStreamPacket* RPCServicePacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {
            InitSessionPacket* packet = new InitSessionPacket();
            packet->init(header, is);
            return packet;
        }

        case RPCInvocationOneWayPacket::RPC_ONE_WAY_INVOCATION_PACKET_OP:{
            RPCInvocationOneWayPacket* packet = new RPCInvocationOneWayPacket();
            packet->init(header,is);
            return packet;
        }
        
        case RPCInvocationPacket::RPC_INVOCATION_PACKET_OP:{
            RPCInvocationPacket* packet = new RPCInvocationPacket();
            packet->init(header,is);
            return packet;
        }
        
        case RPCInvocationReplyPacket::RPC_INVOCATION_REPLY_PACKET_OP:{
            RPCInvocationReplyPacket* packet = new RPCInvocationReplyPacket();
            packet->init(header,is);
            return packet;
        }
        
        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: RPCServicePacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}
