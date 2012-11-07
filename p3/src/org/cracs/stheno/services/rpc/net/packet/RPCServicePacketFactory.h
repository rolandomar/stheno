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
 * File:   RPCServicePacketFactory.h
 * Author: rmartins
 *
 * Created on January 21, 2011, 4:17 PM
 */

#ifndef RPCSERVICEPACKETFACTORY_H
#define	RPCSERVICEPACKETFACTORY_H

#include <euryale/common/io/ByteInputStream.h>
#include <euryale/net/packet/PacketFactory.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <stheno/services/rpc/net/packet/RPCInvocationOneWayPacket.h>
#include <stheno/services/rpc/net/packet/RPCInvocationPacket.h>
#include <stheno/services/rpc/net/packet/RPCInvocationReplyPacket.h>


class RPCServicePacketFactory: public StreamChannelPacketFactory{
public:
    RPCServicePacketFactory(){}
    virtual ~RPCServicePacketFactory(){}

    SthenoStreamPacket* createPacket(SthenoStreamHeader* header, ByteInputStream& is);
};


#endif	/* RPCSERVICEPACKETFACTORY_H */

