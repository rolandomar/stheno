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
 * File:   ActuatorServicePacketFactory.h
 * Author: rmartins
 *
 * Created on February 4, 2011, 12:09 PM
 */

#ifndef ACTUATORSERVICEPACKETFACTORY_H
#define	ACTUATORSERVICEPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>

#include <stheno/services/actuator/net/packet/ActionPacket.h>
#include <stheno/services/actuator/net/packet/ActionReplyPacket.h>
#include <stheno/services/actuator/net/packet/StartActuatorServicePacket.h>

class ActuatorServicePacketFactory:public StreamChannelPacketFactory{
public:
    ActuatorServicePacketFactory();    
    virtual ~ActuatorServicePacketFactory();
    
    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};

#endif	/* ACTUATORSERVICEPACKETFACTORY_H */

