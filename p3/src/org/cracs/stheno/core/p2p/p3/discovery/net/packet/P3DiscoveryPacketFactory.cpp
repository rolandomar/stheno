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

#include "P3DiscoveryPacketFactory.h"


SthenoStreamPacket* P3DiscoveryPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case P3DiscoveryQueryPacket::P3DISCOVERY_QUERY_PACKET:
        {
            P3DiscoveryQueryPacket* packet = new P3DiscoveryQueryPacket();         
            packet->init(header, is);
            return packet;
        }
        
        case P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET:
        {
            P3DiscoveryQueryReplyPacket* packet = new P3DiscoveryQueryReplyPacket();         
            packet->init(header, is);
            return packet;
        }
        
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {
            InitSessionPacket* packet = new InitSessionPacket();         
            packet->init(header, is);
            return packet;
        }
        
        default:{
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) ERROR: P3DiscoveryPacketFactory: header->getType()=%d\n"),header->getType()));
            return 0;
        }
            
           
    }
}

