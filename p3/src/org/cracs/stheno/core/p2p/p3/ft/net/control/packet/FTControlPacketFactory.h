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
 * File:   FTControlPacketFactory.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 11:34 AM
 */

#ifndef FTCONTROLPACKETFACTORY_H
#define	FTCONTROLPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>

#include <stheno/core/p2p/p3/ft/net/control/packet/FTAddReplicaPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTAddReplicaReplyPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTInitReplicaPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTInitReplicaReplyPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTRemoveReplicaPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTRemoveReplicaReplyPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTPrebindPacket.h>
#include <stheno/core/p2p/p3/ft/net/control/packet/FTPrebindReplyPacket.h>



class FTControlPacketFactory: public StreamChannelPacketFactory{
public:
    FTControlPacketFactory(){}
    virtual ~FTControlPacketFactory(){}

    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};

#endif	/* FTCONTROLPACKETFACTORY_H */

