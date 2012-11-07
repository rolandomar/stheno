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

#include "StreamServicePacketFactory.h"

SthenoStreamPacket* StreamServicePacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case StartStreamServicePacket::START_STREAM_SERVICE_PACKET_OP:
        {
            StartStreamServicePacket* packet = new StartStreamServicePacket();
            packet->init(header, is);
            return packet;
        }
        
        case ResumeStreamServicePacket::RESUME_STREAM_SERVICE_PACKET_OP:
        {
            ResumeStreamServicePacket* packet = new ResumeStreamServicePacket();
            packet->init(header, is);
            return packet;
        }
        
        case ResumeStreamReplyPacket::RESUME_STREAM_SERVICE_REPLY_PACKET_OP:
        {
            ResumeStreamReplyPacket* packet = new ResumeStreamReplyPacket();
            packet->init(header, is);
            return packet;
        }
        
        case StreamFramePacket::STREAM_FRAME_PACKET_OP:
        {
            StreamFramePacket* packet = new StreamFramePacket();
            packet->init(header, is);
            return packet;
        }
        
        case StreamMissingFramePacket::STREAM_MISSING_FRAME_PACKET_OP:
        {
            StreamMissingFramePacket* packet = new StreamMissingFramePacket();
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

