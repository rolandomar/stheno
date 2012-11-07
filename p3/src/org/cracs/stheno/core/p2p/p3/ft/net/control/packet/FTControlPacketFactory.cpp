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
 * File:   FTControlPacketFactory.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 11:34 AM
 */

#include "FTControlPacketFactory.h"
#include "FTAddReplicaPacket.h"
#include "FTRemoveReplicaPacket.h"
#include "FTInitReplicaPacket.h"

SthenoStreamPacket* FTControlPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
            case FTAddReplicaPacket::ADD_REPLICA_PACKET_OP:
            {
                FTAddReplicaPacket* packet = new FTAddReplicaPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTAddReplicaReplyPacket::ADD_REPLICA_REPLY_PACKET_OP:
            {
                FTAddReplicaReplyPacket* packet = new FTAddReplicaReplyPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTRemoveReplicaPacket::REMOVE_REPLICA_PACKET_OP:
            {
                FTRemoveReplicaPacket* packet = new FTRemoveReplicaPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTRemoveReplicaReplyPacket::REMOVE_REPLICA_REPLY_PACKET_OP:
            {
                FTRemoveReplicaReplyPacket* packet = new FTRemoveReplicaReplyPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTInitReplicaPacket::INIT_REPLICA_PACKET_OP:
            {
                FTInitReplicaPacket* packet = new FTInitReplicaPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTInitReplicaReplyPacket::INIT_REPLICA_REPLY_PACKET_OP:
            {
                FTInitReplicaReplyPacket* packet = new FTInitReplicaReplyPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTPrebindPacket::PREBIND_PACKET_OP:
            {
                FTPrebindPacket* packet = new FTPrebindPacket();
                packet->init(header, is);
                return packet;
            }
            
            case FTPrebindReplyPacket::PREBIND_REPLY_PACKET_OP:
            {
                FTPrebindReplyPacket* packet = new FTPrebindReplyPacket();
                packet->init(header, is);
                return packet;
            }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: FTControlPacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}
