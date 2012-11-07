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
 * File:   CellPacketFactory.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 4:11 PM
 */

#ifndef CELLPACKETFACTORY_H
#define	CELLPACKETFACTORY_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <euryale/common/io/ByteInputStream.h>

#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/LeaveMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/LeaveMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/RebindMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/RebindMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/UpdateInfoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/UpdateInfoReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellInitSession.h>

#include <euryale/net/packet/PacketFactory.h>

class CellPacketFactory: public PacketFactory<SthenoPacket,SthenoHeader> {
public:
    CellPacketFactory(){}
    virtual ~CellPacketFactory(){}


    SthenoPacket* createPacket(SthenoHeader* header, ByteInputStream& is){
        switch(header->m_messageType){                        
            case CellInitSession::CELL_INIT_SESSION_PACKET_OP:{
                CellInitSession* packet = new CellInitSession();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JoinCellPacket %lu\n",is.length());
                packet->init(header,is);
                return packet;
            }
            
            case JOIN_CELL_PACKET:{
                JoinCellPacket* packet = new JoinCellPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JoinCellPacket %lu\n",is.length());
                packet->init(header,is);
                return packet;
            }
            case JOIN_CELL_REPLY_PACKET:{
                JoinCellReplyPacket* packet = new JoinCellReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case JOIN_MESH_PACKET:{
                JoinMeshPacket* packet = new JoinMeshPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JoinCellPacket %lu\n",is.length());
                packet->init(header,is);
                return packet;
            }
            case JOIN_MESH_REPLY_PACKET:{
                JoinMeshReplyPacket* packet = new JoinMeshReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case LEAVE_MESH_PACKET:{
                LeaveMeshPacket* packet = new LeaveMeshPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JoinCellPacket %lu\n",is.length());
                packet->init(header,is);
                return packet;
            }
            case LEAVE_MESH_REPLY_PACKET:{
                LeaveMeshReplyPacket* packet = new LeaveMeshReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case REBIND_MESH_PACKET:{
                RebindMeshPacket* packet = new RebindMeshPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JoinCellPacket %lu\n",is.length());
                packet->init(header,is);
                return packet;
            }
            case REBIND_MESH_REPLY_PACKET:{
                RebindMeshReplyPacket* packet = new RebindMeshReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case UPDATEINFO_MESH_PACKET:{
                UpdateInfoPacket* packet = new UpdateInfoPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            case UPDATEINFO_MESH_REPLY_PACKET:{
                UpdateInfoReplyPacket* packet = new UpdateInfoReplyPacket();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET %lu\n",is.length());
                packet->init(header,is);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellPacketFactory: JOIN_CELL_REPLY_PACKET after deserialize %lu\n",is.length());
                return packet;
            }
            default:
                return 0;
        }
    }
private:

};

#endif	/* CELLPACKETFACTORY_H */

