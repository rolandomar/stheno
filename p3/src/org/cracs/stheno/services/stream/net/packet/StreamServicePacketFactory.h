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
 * File:   P3DiscoveryPacketFactory.h
 * Author: rmartins
 *
 * Created on September 22, 2010, 11:09 AM
 */

#ifndef STREAMSERVICEPACKETFACTORY_H
#define	STREAMSERVICEPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <stheno/services/stream/net/packet/StartStreamServicePacket.h>
#include <stheno/services/stream/net/packet/ResumeStreamServicePacket.h>
#include <stheno/services/stream/net/packet/ResumeStreamReplyPacket.h>
#include <stheno/services/stream/net/packet/StreamFramePacket.h>
#include <stheno/services/stream/net/packet/StreamMissingFramePacket.h>


class StreamServicePacketFactory: public StreamChannelPacketFactory{//public PacketFactory<SthenoStreamPacket,SthenoStreamHeader>  {
public:
    StreamServicePacketFactory(){}
    virtual ~StreamServicePacketFactory(){}

    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};


#endif	/* STREAMSERVICEPACKETFACTORY_H */

