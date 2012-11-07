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
 * File:   P3DiscoveryPacketFactory.cpp
 * Author: rmartins
 * 
 * Created on September 22, 2010, 11:09 AM
 */

#include "ActuatorPacketFactory.h"

SthenoStreamPacket* ActuatorPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {
            InitSessionPacket* packet = new InitSessionPacket();         
            packet->init(header, is);
            return packet;
        }
        case ActuatorActionPacket::ACTION_PACKET_OP:
        {
            ActuatorActionPacket* packet = new ActuatorActionPacket();
            packet->init(header, is);
            return packet;
        }
        
        case ActuatorActionReplyPacket::ACTION_REPLY_PACKET_OP:
        {
            ActuatorActionReplyPacket* packet = new ActuatorActionReplyPacket();
            packet->init(header, is);
            return packet;
        }
                
        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: ActuatorPacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}

