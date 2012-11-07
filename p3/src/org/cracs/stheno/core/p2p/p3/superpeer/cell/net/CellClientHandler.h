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
 * File:   CellClientHandler.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 1:00 PM
 */

#ifndef CELLCLIENTHANDLER_H
#define	CELLCLIENTHANDLER_H


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
#include <euryale/net/RequestEngine.h>

class CellClientHandler : public PacketChannel<SthenoPacket, SthenoHeader,
//CellPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    CellClientHandler();

    CellClientHandler(UUIDPtr& uuid, Cell* cell, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    int open(void *arg = 0) {
        int ret = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
        return ret;
    }

    virtual int close(u_long flags = 0) {
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
            if (!m_close) {
                m_requests.clearAll();
            }
        }
        return PacketChannel<SthenoPacket, SthenoHeader, ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual ACE_Reactor *reactor(void) const {
        ACE_Reactor* reactor = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::reactor() - (%@)\n"), reactor));
        return reactor;
    }

    virtual void reactor(ACE_Reactor *reactor) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
        PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
    }

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    bool joinMesh(ACE_Message_Block*& mb) {
        mb = 0;
        JoinCellPacket *packet = 0;
        CellIDPtr cellIDPtr;
        m_cell->getCellID(cellIDPtr);
        UUIDPtr cellID(new CellID(cellIDPtr.get()));
        SAPInfoPtr discoverySAP;
        SAPInfoPtr meshSAP;
        SAPInfoPtr ftSAP;
        UUIDPtr uuid;
        m_cell->getPID(uuid);
        UUIDPtr fid;
        m_cell->getFID(fid);

        m_cell->getDiscoverySAPInfo(uuid, discoverySAP);
        m_cell->getMeshSAPInfo(uuid, meshSAP);
        m_cell->getFTSAPInfo(uuid, ftSAP);
        if (m_debugClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellClientHandler:joinCell() - SRC(%s,%s) \n\tDST(%s,%s)\n\t DiscoverySAP(%@)\n"),
                    uuid.get()->toString().c_str(),
                    cellID->toString().c_str(),
                    m_uuid->toString().c_str(),
                    cellID->toString().c_str(),
                    discoverySAP.get()
                    ));
        }
        EndpointPtr cellSAP;
        m_cell->getSAP(cellSAP);
        EndpointPtr followerSAP;
        m_cell->getFollowerSAP(followerSAP);
        packet = new JoinCellPacket(uuid,
                cellID, //CellID::INVALID_CELL_ID_UUIDPTR,
                m_uuid,
                cellID,
                0,
                cellSAP,
                followerSAP,
                discoverySAP,
                meshSAP,
                ftSAP);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            if (m_debugClientHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellClientHandler::jonCell() - request failed!\n")));
            }
            return 0;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(JOIN_CELL_TIMEOUT_MS);
        timeout += delta;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellClientHandler::joinCell() - before future.\n")));
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellClientHandler::joinCell() - after future\n")));
        if (results != 0 && results->size() > 0) {
            JoinCellReplyPacket* replyPacket = static_cast<JoinCellReplyPacket*> (results->front());
            bool status = replyPacket->getStatus();
            mb = replyPacket->deattachState();
            /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::joinCell() - inside loop, msg:%l %l\n"),
            replyPacket->deattachState()->total_length(), mb->total_length()));*/
            ListHelper<SthenoPacket*>::deleteList(results);
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellClientHandler::joinCell() - No response\n")));
        }

        return false;

    }

    bool updateInfo(InfoUpdatePtr& updateInfoPtr) {
        SthenoPacket *packet = 0;
        UUIDPtr runtimeUUID;
        m_cell->getPID(runtimeUUID);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);
        UUIDPtr srcCellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(runtimeCellID.get()));
        StreamSize ss;
        updateInfoPtr->serialize(ss);
        ByteOutputStream os(ss.total_length());
        updateInfoPtr->serialize(os);
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
        RDR::consolidate(mb, &os.start_);
        packet = new UpdateInfoPacket(runtimeUUID,
                srcCellID,
                m_uuid,
                dstCellID,
                0,
                mb);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            if (m_debugClientHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellClientHandler:updateInfo(): ------------>REquest failed\n")));
            }
            return 0;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(JOIN_CELL_TIMEOUT_MS);
        timeout += delta;
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
        if (results != 0 && results->size() > 0) {
            UpdateInfoReplyPacket* replyPacket = static_cast<UpdateInfoReplyPacket*> (results->front());
            bool status = replyPacket->getUpdateResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellClientHandler::updateInfo() - No response\n")));
        }

        return false;
    }


    //packet interface

    virtual int handlePacket(SthenoPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {
                /*case JOIN_MESH_PACKET:
                {
                    JoinMeshPacket* joinMeshPacket = static_cast<JoinMeshPacket*> (packet);
                    return m_cell->handleCoordinatorJoinMesh(this, joinMeshPacket);
                }
                case LEAVE_MESH_PACKET:
                {
                    LeaveMeshPacket* leaveMeshPacket = static_cast<LeaveMeshPacket*> (packet);
                    return m_cell->handleCoordinatorLeaveMesh(this, leaveMeshPacket);
                }
                case REBIND_MESH_PACKET:
                {
                    RebindMeshPacket* rebindMeshPacket = static_cast<RebindMeshPacket*> (packet);
                    return m_cell->handleCoordinatorRebindMesh(this, rebindMeshPacket);
                }

                case UPDATEINFO_MESH_PACKET:
                {
                    UpdateInfoPacket* updateInfoPacket = static_cast<UpdateInfoPacket*> (packet);
                    return m_cell->handleCoordinatorUpdateInfo(this, updateInfoPacket);
                }*/
            case JOIN_CELL_REPLY_PACKET:
            {
                JoinCellReplyPacket* replyPacket = static_cast<JoinCellReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getRequestID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }
            case UPDATEINFO_MESH_REPLY_PACKET:
            {
                UpdateInfoReplyPacket* replyPacket = static_cast<UpdateInfoReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getReplyPacketID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }
            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        //}
        return 0;
    }

    virtual RequestEngine<SthenoPacket*>::RequestPtr* sendRequest(SthenoPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (m_close) {
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestEngine<SthenoPacket*>::RequestPtr* request =
                new RequestEngine<SthenoPacket*>::RequestPtr(new Request<SthenoPacket*>(&m_requests, id, timeout));
        m_requests.bind(request);
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellClientHandler:sendRequest() - ID(%d)\n"), id));
            delete request;
            return 0;
        }
        return request;
    }

    virtual ~CellClientHandler();

    UUIDPtr& getPID() {
        return m_uuid;
    }
protected:
    bool m_debugClientHandler;
    UUIDPtr m_uuid;
    RequestEngine<SthenoPacket*> m_requests;
    Cell* m_cell;

};

typedef ACE_Strong_Bound_Ptr<CellClientHandler, ACE_Recursive_Thread_Mutex> CellClientHandlerPtr;

#endif	/* CELLCLIENTHANDLER_H */

