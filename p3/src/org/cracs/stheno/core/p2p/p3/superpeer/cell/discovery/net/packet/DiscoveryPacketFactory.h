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
 * File:   DiscoveryPacketFactory.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 27, 2010, 2:41 PM
 */

#ifndef P3CELLDISCOVERYPACKETFACTORY_H
#define	P3CELLDISCOVERYPACKETFACTORY_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <euryale/common/io/ByteInputStream.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/P3DiscoveryPackets.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/CellNotifyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/FindCoordinatorPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/CoordinatorReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorReply.h>


class P3CellDiscoveryPacketFactory {
public:
    P3CellDiscoveryPacketFactory(){}
    virtual ~P3CellDiscoveryPacketFactory(){}


    static SthenoPacket* createPacket(SthenoHeader* header, ByteInputStream& is){
        switch(header->m_messageType){
            case REQUEST_CELL_PACKET:{
                RequestCellPacket* packet = new RequestCellPacket();
                packet->init(header,is);
                return packet;
            }
            case REQUEST_CELL_REPLY_PACKET:{
                RequestCellReplyPacket* packet = new RequestCellReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)DiscoveryPacketFactory: REQUEST_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)DiscoveryPacketFactory: REQUEST_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case NOTIFY_PACKET:{
                CellNotifyPacket* packet = new CellNotifyPacket();
                packet->init(header,is);
                return packet;
            }
            case FIND_COORDINATOR_PACKET:{
                FindCoordinatorPacket* packet = new FindCoordinatorPacket();
                packet->init(header,is);
                return packet;
            }
            case FIND_COORDINATOR_REPLY_PACKET:{
                CoordinatorReplyPacket* packet = new CoordinatorReplyPacket();
                packet->init(header,is);
                return packet;
            }
            case REQUEST_COORDINATOR_PACKET: {
                RequestCoordinatorPacket* packet = new RequestCoordinatorPacket();
                packet->init(header,is);
                return packet;
            }
            
            case REQUEST_COORDINATOR_REPLY_PACKET: {
                RequestCoordinatorReplyPacket* packet = new RequestCoordinatorReplyPacket();
                packet->init(header,is);
                return packet;
            }


            default:
                return 0;
        }
    }

};

#endif	/* P3CELLDISCOVERYPACKETFACTORY_H */

