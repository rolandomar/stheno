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
 * File:   CellSvcHandler.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 12:59 PM
 */

#ifndef CELLSVCHANDLER_H
#define	CELLSVCHANDLER_H

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Reactor.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Stream.h"
#include "ace/LSOCK_Connector.h"
#include "ace/Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/net/PacketChannel.h>
#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshReplyPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinCellReplyPacket.h>
#include <euryale/net/RequestEngine.h>

class CellSvcHandler : public PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    static const int MAX_OPEN_TIMEOUT_MS = 2000;
    bool m_debugCellSvcHandler;

    CellSvcHandler(Cell* cell, //bool asyncRead, bool asyncWrite,
            //bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0) :
    PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
    ACE_SOCK_Stream, ACE_MT_SYNCH>(new CellPacketFactory(), false, false,
    ec, reserve, qos, closeListener)/*, m_requests(0)*/ {
        m_debugCellSvcHandler = RuntimeConfiguration::isClassDebugged("CellSvcHandler");
        m_cell = cell;
        //m_requests = new RequestEngine<SthenoPacket*>();
        m_type = -1;
    }

    virtual int open(void *arg = 0) {
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::open\n")));
        }
        int ret = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        if (ret == -1) {
            if (m_debugCellSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::open failed\n")));
            }
            return -1;
        }
        //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
        ACE_Time_Value receiveTimeout;
        receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

        SthenoPacket* recvPacket = 0;
        //receivePacket(recvPacket, &receiveTimeout);
        receivePacket(recvPacket, 0);
        if (recvPacket == 0) {
            if (m_debugCellSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - no init packet\n")));
            }
            return -1;
        }
        switch (recvPacket->getPacketHeader()->getType()) {
            case JOIN_MESH_PACKET:
            {//from children
                JoinMeshPacket* joinMeshPacket = static_cast<JoinMeshPacket*> (recvPacket);
                m_pid = joinMeshPacket->getPacketHeader()->getSrcPID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
                }
                m_fid = joinMeshPacket->getPacketHeader()->getSrcFID();
                UUID* uuid = new UUID(*joinMeshPacket->getPacketHeader()->getSrcFID().get());
                CellIDPtr cellID(new CellID(uuid));
                m_cellID = cellID;
                m_type = joinMeshPacket->getType();
                if (m_cell->handleJoinMesh(this, joinMeshPacket) != 0) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - processJoinCell failde\n")));
                    }
                    return -1;
                }
                //this->asynchronous(true, true);
                return 0;
            }
            case JOIN_CELL_PACKET://from sibling
            {
                JoinCellPacket* joinCellPacket = static_cast<JoinCellPacket*> (recvPacket);
                m_pid = joinCellPacket->getPacketHeader()->getSrcPID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
                }
                UUID* uuid = new UUID(*joinCellPacket->getPacketHeader()->getSrcFID().get());
                CellIDPtr cellID(new CellID(uuid));
                m_cellID = cellID;
                m_fid = joinCellPacket->getPacketHeader()->getSrcFID();
                m_followerEndpoint = joinCellPacket->getFollowerEndpoint();
                m_type = P3PeerInfo::SUPERPEER;
                if (m_cell->handleJoinCell(this, joinCellPacket) != 0) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - processJoinCell failde\n")));
                    }
                    return -1;
                }
                //this->asynchronous(true, true);
                return 0;
            }
            case REBIND_MESH_PACKET:
            {//from children
                RebindMeshPacket* rebindMeshPacket = static_cast<RebindMeshPacket*> (recvPacket);
                m_pid = rebindMeshPacket->getPacketHeader()->getSrcPID();
                m_fid = rebindMeshPacket->getPacketHeader()->getSrcFID();
                m_followerEndpoint = rebindMeshPacket->getFollowerEndpoint();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
                }
                UUID* uuid = new UUID(*rebindMeshPacket->getPacketHeader()->getSrcFID().get());
                CellIDPtr cellID(new CellID(uuid));
                m_cellID = cellID;
                m_type = P3PeerInfo::SUPERPEER;
                if (m_cell->handleRebindMesh(this, rebindMeshPacket) != 0) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - REBIND_MESH_PACKET failde\n")));
                    }
                    return -1;
                }
                //this->asynchronous(true, true);
                return 0;
            }
            case UPDATEINFO_MESH_PACKET:
            {
                UpdateInfoPacket* updateInfoPacket = static_cast<UpdateInfoPacket*> (recvPacket);
                if (m_cell->handleUpdateInfo(this, updateInfoPacket) != 0) {
                    delete recvPacket;
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - UPDATEINFO_MESH_PACKET failde\n")));
                    }
                    return -1;
                }
            }

            default:
            {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
                close();
                return -1;
            }
        }
    }

    virtual int close(u_long flags = 0) {
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
            if (!m_close) {
                /*if (m_requests != 0) {
                    delete m_requests;
                    m_requests = 0;
                }*/
                m_requests.clearAll();
            }
        }
        return PacketChannel<SthenoPacket, SthenoHeader, ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual int handlePacket(SthenoPacket* packet) {
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() type(%d)\n"), packet->getPacketHeader()->getType()));
        }
        switch (packet->getPacketHeader()->getType()) {
            case JOIN_CELL_PACKET:
            {
                JoinCellPacket* joinCellPacket = static_cast<JoinCellPacket*> (packet);
                return m_cell->handleJoinCell(this, joinCellPacket);
            }
            case JOIN_MESH_PACKET:
            {
                JoinMeshPacket* joinMeshPacket = static_cast<JoinMeshPacket*> (packet);
                return m_cell->handleJoinMesh(this, joinMeshPacket);
            }

            case LEAVE_MESH_PACKET:
            {
                LeaveMeshPacket* leaveMeshPacket = static_cast<LeaveMeshPacket*> (packet);
                return m_cell->handleLeaveMesh(this, leaveMeshPacket);
            }
            case JOIN_MESH_REPLY_PACKET:
            {

                JoinMeshReplyPacket* joinMeshReplyPacket = static_cast<JoinMeshReplyPacket*> (packet);
                UInt id = joinMeshReplyPacket->getRequestID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - PeerID=%s ID(%d) JOIN_MESH_REPLY_PACKET\n"),
                            this->getPID()->toString().c_str(),
                            id));
                }
                if (m_requests.setReply(id, packet) == -1) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - failed to set future ID(%d) JOIN_MESH_REPLY_PACKET\n"), id));
                    }
                    delete packet;
                    return -1;
                }
                return 0;
            }
            case LEAVE_MESH_REPLY_PACKET:
            {

                LeaveMeshReplyPacket* leaveMeshReplyPacket = static_cast<LeaveMeshReplyPacket*> (packet);
                UInt id = leaveMeshReplyPacket->getRequestID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - PeerID=%s ID(%d) LEAVE_MESH_REPLY_PACKET\n"),
                            this->getPID()->toString().c_str(),
                            id));
                }
                if (m_requests.setReply(id, packet) == -1) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - failed to set future ID(%d) LEAVE_MESH_REPLY_PACKET\n"), id));
                    }
                    delete packet;
                    return -1;
                }
                return 0;
            }
            case REBIND_MESH_REPLY_PACKET:
            {
                RebindMeshReplyPacket* joinMeshReplyPacket = static_cast<RebindMeshReplyPacket*> (packet);
                UInt id = joinMeshReplyPacket->getRequestID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - PeerID=%s ID(%d) REBIND_MESH_REPLY_PACKET\n"),
                            this->getPID()->toString().c_str(),
                            id));
                }
                if (m_requests.setReply(id, packet) == -1) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - failed to set future ID(%d) REBIND_MESH_REPLY_PACKET\n"), id));
                    }
                    delete packet;
                    return -1;
                }
                return 0;
            }

            case UPDATEINFO_MESH_PACKET:
            {
                UpdateInfoPacket* replyPacket = static_cast<UpdateInfoPacket*> (packet);
                return m_cell->handleUpdateInfo(this, replyPacket);
            }

            case UPDATEINFO_MESH_REPLY_PACKET:
            {
                UpdateInfoReplyPacket* replyPacket = static_cast<UpdateInfoReplyPacket*> (packet);
                UInt id = replyPacket->getReplyPacketID();
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - PeerID=%s ID(%d) UPDATEINFO_MESH_REPLY_PACKET\n"),
                            this->getPID()->toString().c_str(),
                            id));
                }
                if (m_requests.setReply(id, packet) == -1) {
                    if (m_debugCellSvcHandler) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: CellSvcHandler::handlePacket() - failed to set future ID(%d) UPDATEINFO_MESH_REPLY_PACKET\n"), id));
                    }
                    delete packet;
                    return -1;
                }
                return 0;
            }
            default:
                if (m_debugCellSvcHandler) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: CellSvcHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                }
                delete packet;
                return 0;
        }
    }

    virtual ~CellSvcHandler();

    UUIDPtr& getPID() {
        return m_pid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    CellIDPtr& getCellID() {
        return m_cellID;
    }

    bool isChildren() {
        CellIDPtr cellID;
        m_cell->getCellID(cellID);
        return !((*cellID).isEqual(*m_cellID));
    }

    RequestEngine<SthenoPacket*>::RequestPtr* sendRequest(SthenoPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_Message_Block* mb = serializePacket(packet);
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler:sendRequest() - ID=%d UUID=%s CELLID=%s\n"), packet->getPacketHeader()->getPacketID(),
                    this->getPID()->toString().c_str(), m_cellID->toString().c_str()));
        }
        UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestEngine<SthenoPacket*>::RequestPtr* request =
                new RequestEngine<SthenoPacket*>::RequestPtr(new Request<SthenoPacket*>(&m_requests, id, timeout));
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: CellSvcHandler::sendRequest() - before bind PeerID=%s ID=%d\n"),
                    this->getPID()->toString().c_str(),
                    packet->getPacketHeader()->getPacketID()));
        }
        m_requests.bind(request);
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: CellSvcHandler::sendRequest() - after bind PeerID=%s ID=%d\n"),
                    this->getPID()->toString().c_str(),
                    packet->getPacketHeader()->getPacketID()));
        }
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: CellSvcHandler::sendRequest() - sent PeerID=%s ID=%d\n"),
                    this->getPID()->toString().c_str(),
                    packet->getPacketHeader()->getPacketID()));
        }
        if (ret == -1) {
            if (m_debugCellSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR:: CellSvcHandler::sendRequest() - send PeerID=%s ID(%d) failed!\n"),
                        this->getPID()->toString().c_str(),
                        id));
            }
            delete request;
            return 0;
        }
        if (m_debugCellSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: CellSvcHandler::sendRequest() - END PeerID=%s ID=%d\n"),
                    this->getPID()->toString().c_str(),
                    packet->getPacketHeader()->getPacketID()));
        }
        return request;
    }

    int getType() {
        return m_type;
    }

    bool isLeaf() {
        return m_type == P3PeerInfo::SENSOR || m_type == P3PeerInfo::MONITOR;
    }

    EndpointPtr& getFollowerEndpoint() {
        return m_followerEndpoint;
    }

protected:
    Cell* m_cell;
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    CellIDPtr m_cellID;
    EndpointPtr m_followerEndpoint;
    //we need this
    int m_type;
    RequestEngine<SthenoPacket*> m_requests;


};


#endif	/* CELLSVCHANDLER_H */

