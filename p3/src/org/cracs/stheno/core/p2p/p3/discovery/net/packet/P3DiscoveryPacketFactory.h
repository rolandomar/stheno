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

#ifndef P3DISCOVERYPACKETFACTORY_H
#define	P3DISCOVERYPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
//#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryQueryPacket.h>
#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryQueryReplyPacket.h>
#include <stheno/core/p2p/net/packet/InitSessionPacket.h>

#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>

class P3DiscoveryPacketFactory: public StreamChannelPacketFactory{//public PacketFactory<SthenoStreamPacket,SthenoStreamHeader>  {
public:
    P3DiscoveryPacketFactory(){}
    virtual ~P3DiscoveryPacketFactory(){}
    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};


#endif	/* P3DISCOVERYPACKETFACTORY_H */

