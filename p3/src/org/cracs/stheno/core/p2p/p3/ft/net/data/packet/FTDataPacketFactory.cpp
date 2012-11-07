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
 * File:   FTDataPacketFactory.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 5:12 PM
 */

#include "FTDataPacketFactory.h"
#include "FTDataReplicationPacket.h"
#include "FTDataReplicationReplyPacket.h"
#include "FTInitDataGroupPacket.h"

SthenoStreamPacket* FTDataPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case FTDataReplicationPacket::DATA_REPLICATION_PACKET_OP:
        {
            FTDataReplicationPacket* packet = new FTDataReplicationPacket();
            packet->init(header, is);
            return packet;
        }
        case FTDataReplicationReplyPacket::DATA_REPLICATION_REPLY_PACKET_OP:
        {
            FTDataReplicationReplyPacket* packet = new FTDataReplicationReplyPacket();
            packet->init(header, is);
            return packet;
        }
        /***** Init related packets***/
        case FTInitDataGroupPacket::INIT_DATA_GROUP_PACKET_OP:
        {
            FTInitDataGroupPacket* packet = new FTInitDataGroupPacket();
            packet->init(header, is);
            return packet;
        }
        case FTInitDataGroupReplyPacket::INIT_DATA_GROUP_REPLY_PACKET_OP:
        {
            FTInitDataGroupReplyPacket* packet = new FTInitDataGroupReplyPacket();
            packet->init(header, is);
            return packet;
        }

        case FTDataPrebindPacket::FT_DATA_PREBIND_PACKET_OP:
        {
            FTDataPrebindPacket* packet = new FTDataPrebindPacket();
            packet->init(header, is);
            return packet;
        }

        case FTDataPrebindReplyPacket::FT_DATA_PREBIND_REPLY_PACKET_OP:
        {
            FTDataPrebindReplyPacket* packet = new FTDataPrebindReplyPacket();
            packet->init(header, is);
            return packet;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: FTDataPacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}