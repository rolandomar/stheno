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

#ifndef STREAMPACKETFACTORY_H
#define	STREAMPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <stheno/core/p2p/p3/leaf/stream/net/packet/StartStreamPacket.h>
#include <stheno/core/p2p/p3/leaf/stream/net/packet/StartStreamReplyPacket.h>
#include <stheno/core/p2p/p3/leaf/stream/net/packet/FramePacket.h>
#include <stheno/core/p2p/p3/leaf/stream/net/packet/MissingFramePacket.h>


class StreamPacketFactory: public StreamChannelPacketFactory{//public PacketFactory<SthenoStreamPacket,SthenoStreamHeader>  {
public:
    StreamPacketFactory(){}
    virtual ~StreamPacketFactory(){}

    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};


#endif	/* STREAMPACKETFACTORY_H */

