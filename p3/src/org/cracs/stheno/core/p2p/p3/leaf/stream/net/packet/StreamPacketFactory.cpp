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

#include "StreamPacketFactory.h"

SthenoStreamPacket* StreamPacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case StartStreamPacket::START_STREAM_PACKET_OP:
        {
            StartStreamPacket* packet = new StartStreamPacket();
            packet->init(header, is);
            return packet;
        }
        
        case StartStreamReplyPacket::START_STREAM_REPLY_PACKET:
        {
            StartStreamReplyPacket* packet = new StartStreamReplyPacket();
            packet->init(header, is);
            return packet;
        }
        
        case FramePacket::FRAME_PACKET_OP:
        {
            FramePacket* packet = new FramePacket();
            packet->init(header, is);
            return packet;
        }
        
        case MissingFramePacket::MISSING_FRAME_PACKET_OP:
        {
            MissingFramePacket* packet = new MissingFramePacket();
            packet->init(header, is);
            return packet;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: StreamPacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}

