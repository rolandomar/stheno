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
 * File:   P3MeshPacketFactory.cpp
 * Author: rmartins
 * 
 * Created on September 30, 2010, 6:25 PM
 */

#include "P3MeshPacketFactory.h"

#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServiceReplyPacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServiceReplyPacket.h>

SthenoStreamPacket* P3MeshPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {
            InitSessionPacket* packet = new InitSessionPacket();         
            packet->init(header, is);
            return packet;
        }
        
        case P3MeshCreateServicePacket::P3MESH_CREATESERVICE_PACKET:
        {
            P3MeshCreateServicePacket* packet = new P3MeshCreateServicePacket();         
            packet->init(header, is);
            return packet;
        }
        case P3MeshCreateServiceReplyPacket::P3MESH_CREATESERVICERPELY_PACKET:
        {
            P3MeshCreateServiceReplyPacket* packet = new P3MeshCreateServiceReplyPacket();         
            packet->init(header, is);
            return packet;
        }
        case P3MeshRemoveServicePacket::P3MESH_REMOVESERVICE_PACKET:
        {
            P3MeshRemoveServicePacket* packet = new P3MeshRemoveServicePacket();         
            packet->init(header, is);
            return packet;
        }
        case P3MeshRemoveServiceReplyPacket::P3MESH_REMOVESERVICERPELY_PACKET:
        {
            P3MeshRemoveServiceReplyPacket* packet = new P3MeshRemoveServiceReplyPacket();         
            packet->init(header, is);
            return packet;
        }
        
        default:{
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) ERROR: P3MeshPacketFactory: header->getType()=%d\n"),header->getType()));
            return 0;
        }
            
           
    }
}
