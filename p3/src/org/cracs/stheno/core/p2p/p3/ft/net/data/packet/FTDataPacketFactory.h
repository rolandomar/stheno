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
 * File:   FTDataPacketFactory.h
 * Author: rmartins
 *
 * Created on November 27, 2010, 5:12 PM
 */

#ifndef FTDATAPACKETFACTORY_H
#define	FTDATAPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataReplicationPacket.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataReplicationReplyPacket.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTInitDataGroupPacket.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTInitDataGroupReplyPacket.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataPrebindPacket.h>
#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataPrebindReplyPacket.h>


class FTDataPacketFactory: public StreamChannelPacketFactory{
public:
    FTDataPacketFactory(){}
    virtual ~FTDataPacketFactory(){}

    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};


#endif	/* FTDATAPACKETFACTORY_H */

