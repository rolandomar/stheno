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
 * File:   ActuatorServicePacketFactory.cpp
 * Author: rmartins
 * 
 * Created on February 4, 2011, 12:09 PM
 */

#include "ActuatorServicePacketFactory.h"

ActuatorServicePacketFactory::ActuatorServicePacketFactory() {
}


ActuatorServicePacketFactory::~ActuatorServicePacketFactory() {
}

SthenoStreamPacket* ActuatorServicePacketFactory::createPacket(SthenoStreamHeader* header, ByteInputStream& is) {
    switch (header->getType()) {
        case ActionPacket::ACTION_PACKET_OP:
        {
            ActionPacket* packet = new ActionPacket();
            packet->init(header, is);
            return packet;
        }
        case ActionReplyPacket::ACTION_REPLY_PACKET_OP:
        {
            ActionReplyPacket* packet = new ActionReplyPacket();
            packet->init(header, is);
            return packet;
        }
        case StartActuatorServicePacket::START_ACTUATOR_SERVICE_PACKET_OP:
        {
            StartActuatorServicePacket* packet = new StartActuatorServicePacket();
            packet->init(header, is);
            return packet;
        }        
        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) ERROR: ActuatorServicePacketFactory: header->getType()=%d\n"), header->getType()));
            return 0;
        }


    }
}

