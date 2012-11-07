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
 * File:   ParentCellClientHandler.h
 * Author: rmartins
 *
 * Created on August 25, 2010, 6:33 PM
 */

#ifndef PARENTCELLCLIENTHANDLER_H
#define	PARENTCELLCLIENTHANDLER_H


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
//#include "ace/LSOCK_Acceptor.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch_Traits.h>

#include "ace/Reactor.h"
#include "packet/JoinMeshReplyPacket.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <euryale/net/RequestEngine.h>

class ParentCellClientHandler : public PacketChannel<SthenoPacket, SthenoHeader,
//CellPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    ParentCellClientHandler() {
        throw -1;
    }

    UUIDPtr& getPID() {
        return m_uuid;
    }

    CellIDPtr& getCellID() {
        return m_cellID;
    }

    ParentCellClientHandler(UUIDPtr& uuid, CellIDPtr& cellID,
            Cell* cell, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    int open(void *arg = 0) {
        int ret = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::open - (%d) reactor(%@)\n"), ret, this->reactor()));
        return ret;
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

    virtual ACE_Reactor *reactor(void) const {
        ACE_Reactor* reactor = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::reactor() - (%@)\n"), reactor));
        return reactor;
    }

    virtual void reactor(ACE_Reactor *reactor) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
        PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
    }




    static const int JOIN_CELL_TIMEOUT_MS = 20000;
    static const int LEAVE_CELL_TIMEOUT_MS = 20000;

    bool joinSiblingPeer(int type, UUIDPtr& uuid, CellIDPtr& cellID,
            CellIDPtr& parentCellID, EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP) {
        JoinMeshPacket *packet = 0;
        if (m_debugParentCellClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ParentCellClientHandler:joinCell(): PEER(%s,%s)\n"),
                    uuid->toString().c_str(),
                    cellID->toString().c_str()));
        }

        UUIDPtr runtimeUUID;
        m_cell->getPID(runtimeUUID);
        UUIDPtr fid;
        m_cell->getFID(fid);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);

        UUIDPtr srcCellID(new CellID(*(runtimeCellID.get())));
        UUIDPtr dstCellID(new CellID(*(cellID.get())));
        packet = new JoinMeshPacket(runtimeUUID,
                srcCellID,
                m_uuid,
                dstCellID,
                0,
                type,
                uuid,
                cellID,
                parentCellID,
                endpoint,
                discoverySAP,
                meshSAP,
                ftSAP);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:joinMesh() - Request failed\n")));
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
            JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (results->front());
            bool status = replyPacket->getJoinResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            delete request;
            return status;
        } else {
            delete request;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::joinMesh() - No response\n")));
            return false;
        }
    }

    bool leftSiblingPeer(UUIDPtr& uuid, CellIDPtr& cellID) {
        SthenoPacket *packet = 0;
        if (m_debugParentCellClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ParentCellClientHandler:leftSiblingPeer(): PEER(%s,%s)\n"),
                    uuid->toString().c_str(),
                    cellID->toString().c_str()));
        }
        UUIDPtr runtimeUUID;
        m_cell->getPID(runtimeUUID);
        UUIDPtr fid;
        m_cell->getFID(fid);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);


        UUIDPtr srcCellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(m_cellID.get()));
        packet = new LeaveMeshPacket(runtimeUUID,
                srcCellID,
                m_uuid,
                dstCellID,
                0,
                uuid,
                cellID);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:leftSiblingPeer() - Request failed\n")));
            return false;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(LEAVE_CELL_TIMEOUT_MS);
        timeout += delta;
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
        if (results != 0 && results->size() > 0) {
            LeaveMeshReplyPacket* replyPacket = static_cast<LeaveMeshReplyPacket*> (results->front());
            bool status = replyPacket->getLeaveResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            delete request;
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::leftSiblingPeer() - No response\n")));
            delete request;
            return false;
        }
    }

    bool rebindSiblingPeer(int type, UUIDPtr& uuid, CellIDPtr& cellID,
            CellIDPtr& parentCellID,
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP,
            ACE_Message_Block* mb) {
        SthenoPacket *packet = 0;
        if (m_debugParentCellClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ParentCellClientHandler:joinCell(): PEER(%s,%s)\n"),
                    uuid->toString().c_str(),
                    cellID->toString().c_str()));
        }
        UUIDPtr runtimeUUID;
        m_cell->getPID(runtimeUUID);
        UUIDPtr fid;
        m_cell->getFID(fid);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);

        UUIDPtr srcCellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(m_cellID.get()));
        EndpointPtr followerEndpoint;
        packet = new RebindMeshPacket(runtimeUUID,
                srcCellID,
                m_uuid,
                dstCellID,
                0,
                type,
                uuid,
                cellID,
                parentCellID,
                endpoint,
                followerEndpoint,
                discoverySAP,
                meshSAP,
                ftSAP,
                mb);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        if (request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:joinMesh() - Request failed\n")));
            return false;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(JOIN_CELL_TIMEOUT_MS);
        timeout += delta;
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
        if (results != 0 && results->size() > 0) {
            RebindMeshReplyPacket* replyPacket = static_cast<RebindMeshReplyPacket*> (results->front());
            bool status = replyPacket->getJoinResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            delete request;
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::joinMesh() - No response\n")));
            delete request;
            return false;
        }
    }

    bool joinMesh() {

        JoinMeshPacket *packet = 0;
        UUIDPtr uuid;
        m_cell->getPID(uuid);
        UUIDPtr fid;
        m_cell->getFID(fid);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);

        if (m_debugParentCellClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ParentCellClientHandler:joinCell(): Peer(%s,%s\n"),
                    uuid->toString().c_str(),
                    runtimeCellID->toString().c_str()));
        }

        UUIDPtr cellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(m_cellID.get()));
        SAPInfoPtr discoverySAP;
        m_cell->getDiscoverySAPInfo(uuid, discoverySAP);
        SAPInfoPtr meshSAP;
        m_cell->getMeshSAPInfo(uuid, meshSAP);
        SAPInfoPtr ftSAP;
        m_cell->getFTSAPInfo(uuid, ftSAP);


        int type = P3PeerInfo::SUPERPEER;
        if (m_cell->IsCoordinator()) {
            type = P3PeerInfo::SUPERPEER_COORD;
        }
        CellIDPtr parentCellID;
        m_cell->getParentCellID(parentCellID);
        EndpointPtr cellEndpoint;
        m_cell->getSAP(cellEndpoint);
        packet = new JoinMeshPacket(uuid,
                cellID,
                //m_cell->getCellID(),
                m_uuid,
                dstCellID,
                0,
                type,
                uuid,
                runtimeCellID,
                parentCellID,
                cellEndpoint,
                discoverySAP,
                meshSAP,
                ftSAP
                );
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:joinMesh() - Request failed\n")));
            return false;
        }
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(JOIN_CELL_TIMEOUT_MS);
        timeout += delta;
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
        if (results != 0 && results->size() > 0) {
            JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (results->front());
            bool status = replyPacket->getJoinResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            delete request;
            return status;
        } else {
            delete request;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::joinMesh() - No response\n")));
            return false;
        }
    }

    bool rebindMesh() {
        UUIDPtr uuid;
        m_cell->getPID(uuid);
        UUIDPtr fid;
        m_cell->getFID(fid);
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);
        
        SthenoPacket *packet = 0;
        if (m_debugParentCellClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ParentCellClientHandler:rebind(): Peer(%s,%s\n"),
                    uuid->toString().c_str(),
                    runtimeCellID->toString().c_str()));
        }

        UUIDPtr cellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(m_cellID.get()));
        SAPInfoPtr discoverySAP;
        m_cell->getDiscoverySAPInfo(uuid, discoverySAP);
        SAPInfoPtr meshSAP;
        m_cell->getMeshSAPInfo(uuid, meshSAP);
        SAPInfoPtr ftSAP;
        m_cell->getFTSAPInfo(uuid, ftSAP);

        int type = P3PeerInfo::SUPERPEER;
        if (m_cell->IsCoordinator()) {
            type = P3PeerInfo::SUPERPEER_COORD;
        }
        StreamSize ss;
        m_cell->getCellTree()->serialize(ss);
        ByteOutputStream os(ss.total_length());
        m_cell->getCellTree()->serialize(os);
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
        RDR::consolidate(mb, &os.start_);
        EndpointPtr followerEndpoint;
        CellIDPtr parentCellID;
        m_cell->getParentCellID(parentCellID);
        EndpointPtr cellEndpoint;
        m_cell->getSAP(cellEndpoint);
        packet = new RebindMeshPacket(uuid,
                cellID,
                //m_cell->getCellID(),
                m_uuid,
                dstCellID,
                0,
                type,
                uuid,
                runtimeCellID,
                parentCellID,
                cellEndpoint,
                followerEndpoint,
                discoverySAP,
                meshSAP,
                ftSAP,
                mb);
        RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
        delete packet;
        if (request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:rebind(): Request failed\n")));
            return 0;
        }
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(JOIN_CELL_TIMEOUT_MS);
        timeout += delta;
        list<SthenoPacket*>* results = request->get()->waitFuture(&timeout);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
        if (results != 0 && results->size() > 0) {
            RebindMeshReplyPacket* replyPacket = static_cast<RebindMeshReplyPacket*> (results->front());
            bool status = replyPacket->getJoinResult();
            ListHelper<SthenoPacket*>::deleteList(results);
            delete request;
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::joinMesh() - No response\n")));
            delete request;
            return false;
        }
    }

    bool updateInfo(InfoUpdatePtr& updateInfoPtr) {
        SthenoPacket *packet = 0;
        CellIDPtr runtimeCellID;
        m_cell->getCellID(runtimeCellID);
        UUIDPtr runtimeUUID;
        m_cell->getPID(runtimeUUID);
        UUIDPtr srcCellID(new CellID(runtimeCellID.get()));
        UUIDPtr dstCellID(new CellID(m_cellID.get()));
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
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:updateInfo(): Request failed\n")));
            return false;
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
            delete request;
            return status;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::updateInfo() - No response\n")));
            delete request;
            return false;
        }

    }


    //packet interface

    virtual int handlePacket(SthenoPacket* packet) {
        /*UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::handlePacket request ID(%d)\n", id);
        if (m_requests->setReply(id, packet) == -1) {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::handlePacket: out packet but request was not found!\n");
            delete packet;
            return -1;
        }
        return 0;*/
        switch (packet->getPacketHeader()->getType()) {
            case JOIN_MESH_REPLY_PACKET:
            {
                JoinMeshReplyPacket* joinMeshReplyPacket = static_cast<JoinMeshReplyPacket*> (packet);
                UInt id = joinMeshReplyPacket->getRequestID();
                if (m_requests.setReply(id, packet) == -1) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::handlePacket() - request is gone?(%d)\n"),
                            packet->getPacketHeader()->getType()));
                    delete packet;
                    return -1;
                }
                return 0;
            }
            case LEAVE_MESH_REPLY_PACKET:
            {
                LeaveMeshReplyPacket* leaveMeshReplyPacket = static_cast<LeaveMeshReplyPacket*> (packet);
                UInt id = leaveMeshReplyPacket->getRequestID();
                if (m_requests.setReply(id, packet) == -1) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::handlePacket() - request is gone?(%d)\n"),
                            packet->getPacketHeader()->getType()));
                    delete packet;
                    return -1;
                }
                return 0;
            }
            case REBIND_MESH_REPLY_PACKET:
            {
                RebindMeshReplyPacket* rebindMeshReplyPacket = static_cast<RebindMeshReplyPacket*> (packet);
                UInt id = rebindMeshReplyPacket->getRequestID();
                if (m_requests.setReply(id, packet) == -1) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::handlePacket() - request is gone?(%d)\n"),
                            packet->getPacketHeader()->getType()));
                    delete packet;
                    return -1;
                }
                return 0;
            }

                /*case UPDATEINFO_MESH_PACKET:
                {
                    UpdateInfoReplyPacket* updateInfoReplyPacket = static_cast<UpdateInfoReplyPacket*> (packet);
                    UInt id = updateInfoReplyPacket->getReplyPacketID();
                    if (m_requests->setReply(id, packet) == -1) {
                        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::handlePacket() - request is gone?(%d)\n"),
                        packet->getPacketHeader()->getType()));
                        delete packet;
                        return -1;
                    }
                    return 0;
                }*/

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
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler::handlePacket() we don't know how to handle(%d)\n"),
                        packet->getPacketHeader()->getType()));
                delete packet;
                return -1;
        }


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
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ParentCellClientHandler:sendRequest() - register ID(%d) failed!"), id));
            delete request;
            return 0;
        }
        return request;
    }

    virtual ~ParentCellClientHandler();
protected:
    bool m_debugParentCellClientHandler;
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;
    RequestEngine<SthenoPacket*> m_requests;
    Cell* m_cell;

};

typedef ACE_Strong_Bound_Ptr<ParentCellClientHandler, ACE_Recursive_Thread_Mutex> ParentCellClientHandlerPtr;


#endif	/* PARENTCELLCLIENTHANDLER_H */

