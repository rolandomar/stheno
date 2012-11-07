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
 * File:   LeafClientHandler.cpp
 * Author: rmartins
 * 
 * Created on October 11, 2010, 11:25 AM
 */

#include "LeafClientHandler.h"
#include <euryale/common/Backtrace.h>

LeafClientHandler::~LeafClientHandler() {
    //if (m_debugLeafClientHandler) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:~LeafClientHandler() this=%@\n"), this));
    /*if (m_requests != 0) {
        delete m_requests;
        m_requests = 0;
    }*/
    close();
    //}
    //Backtrace::backstrace();
}

LeafClientHandler::LeafClientHandler(UUIDPtr& uuid, CellIDPtr& cellID,
        P3LeafMesh* mesh, bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new CellPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_cellID(cellID) {
    m_debugLeafClientHandler = RuntimeConfiguration::isClassDebugged("LeafClientHandler");
    m_mesh = mesh;
    //m_requests = new RequestEngine<SthenoPacket*>();
}

LeafClientHandler::LeafClientHandler() {
    throw -1;
}

UUIDPtr& LeafClientHandler::getPID() {
    return m_uuid;
}

CellIDPtr& LeafClientHandler::getCellID() {
    return m_cellID;
}

int LeafClientHandler::open(void *arg) {
    int ret = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)LeafClientHandler::open - (%d) reactor(%@) this=%@\n"), ret, this->reactor(), this));
    return ret;
}

ACE_Reactor* LeafClientHandler::reactor(void) const {
    ACE_Reactor* reactor = PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ParentCellClientHandler::reactor() - (%@)\n"), reactor));
    return reactor;
}

void LeafClientHandler::reactor(ACE_Reactor *reactor) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)LeafClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
    PacketChannel<SthenoPacket, SthenoHeader, // CellPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
}

bool LeafClientHandler::joinSiblingPeer(
        int type,
        UUIDPtr& uuid,
        CellIDPtr& cellID,
        CellIDPtr& parentCellID,
        EndpointPtr& endpoint,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP
        ) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    JoinMeshPacket *packet = 0;
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)LeafClientHandler:joinSiblingPeer(): PEER(%s,%s)\n"),
                runtimeUUID->toString().c_str(),
                cellID->toString().c_str()));
    }

    UUIDPtr srcCellID(new CellID(*(m_cellID.get())));
    UUIDPtr dstCellID(new CellID(*(m_cellID.get())));
    packet = new JoinMeshPacket(m_uuid,
            srcCellID,
            runtimeUUID,
            dstCellID,
            0,
            type,
            uuid,
            cellID,
            parentCellID,
            endpoint,
            discoverySAP,
            meshSAP,
            ftSAP
            );
    RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
    if (request == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:joinSiblingPeer() - Request failed\n")));
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(JOIN_CELL_TIMEOUT_MS);
    timeout += delta;
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafClientHandler:joinSiblingPeer() After future\n")));
    if (results != 0 && results->size() > 0) {
        JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (results->front());
        bool status = replyPacket->getJoinResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::joinSiblingPeer() - No response\n")));
    }

    return false;
}

bool LeafClientHandler::leftSiblingPeer(UUIDPtr& uuid, CellIDPtr& cellID) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    SthenoPacket *packet = 0;
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:leftSiblingPeer(): PEER(%s,%s)\n"),
                uuid->toString().c_str(),
                cellID->toString().c_str()));
    }

    UUIDPtr srcCellID(new CellID(m_cellID.get()));
    UUIDPtr dstCellID(new CellID(m_cellID.get()));
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
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
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:leftSiblingPeer() - Request failed\n")));
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(LEAVE_CELL_TIMEOUT_MS);
    timeout += delta;
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafClientHandler:leftSiblingPeer() After future\n")));
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
    if (results != 0 && results->size() > 0) {
        LeaveMeshReplyPacket* replyPacket = static_cast<LeaveMeshReplyPacket*> (results->front());
        bool status = replyPacket->getLeaveResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::leftSiblingPeer() - No response\n")));
    }

    return false;
}

bool LeafClientHandler::rebindSiblingPeer(
        int type,
        UUIDPtr& uuid,
        CellIDPtr& cellID,
        CellIDPtr& parentCellID,
        EndpointPtr& endpoint,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP,
        ACE_Message_Block* mb) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    SthenoPacket *packet = 0;
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:rebindSiblingPeer(): PEER(%s,%s)\n"),
                uuid->toString().c_str(),
                cellID->toString().c_str()));
    }

    UUIDPtr srcCellID(new CellID(m_cellID.get()));
    UUIDPtr dstCellID(new CellID(m_cellID.get()));
    EndpointPtr followerEndpoint;
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
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
    delete packet;
    if (request == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:rebindSiblingPeer() - Request failed\n")));
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(JOIN_CELL_TIMEOUT_MS);
    timeout += delta;
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafClientHandler:rebindSiblingPeer() After future\n")));
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
    if (results != 0 && results->size() > 0) {
        RebindMeshReplyPacket* replyPacket = static_cast<RebindMeshReplyPacket*> (results->front());
        bool status = replyPacket->getJoinResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::rebindSiblingPeer() - No response\n")));
    }

    return false;
}

bool LeafClientHandler::joinMesh(int type) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    JoinMeshPacket *packet = 0;
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:joinMesh(): Peer(%s,%s\n"),
                m_uuid.get()->toString().c_str(),
                m_cellID->toString().c_str()));
    }

    UUIDPtr cellID(new CellID(m_cellID.get()));
    UUIDPtr dstCellID(new CellID(m_cellID.get()));
    SAPInfoPtr discoverySAP;
    SAPInfoPtr meshSAP;
    SAPInfoPtr ftSAP;
    //int type = P3PeerInfo::SENSOR;
    EndpointPtr nullEndpoint;
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
    UUIDPtr fid;
    m_mesh->getFID(fid);
    packet = new JoinMeshPacket(runtimeUUID,
            cellID,
            m_uuid,
            dstCellID,
            0,
            type,
            runtimeUUID,
            m_cellID,
            m_cellID,
            nullEndpoint,
            discoverySAP,
            meshSAP,
            ftSAP
            );

    RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
    delete packet;
    if (request == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:joinMesh() - request failed\n")));
        return 0;
    }
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(JOIN_CELL_TIMEOUT_MS);
    timeout += delta;
    ace_mon.release();
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafClientHandler:joinMesh() After future\n")));
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
    if (results != 0 && results->size() > 0) {
        JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (results->front());
        bool status = replyPacket->getJoinResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::joinMesh() - No response\n")));
    }

    return false;

}

bool LeafClientHandler::rebindMesh() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    SthenoPacket *packet = 0;
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:rebindMesh(): Peer(%s,%s\n"),
                m_uuid.get()->toString().c_str(),
                m_cellID->toString().c_str()));
    }

    UUIDPtr cellID(new CellID(m_cellID.get()));
    UUIDPtr dstCellID(new CellID(m_cellID.get()));
    int type = P3PeerInfo::SENSOR;
    
    ACE_Message_Block* mb = 0;
    EndpointPtr nullPoint;
    SAPInfoPtr discoverySAP;
    SAPInfoPtr meshSAP;
    SAPInfoPtr ftSAP;
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
    UUIDPtr fid;
    m_mesh->getFID(fid);
    packet = new RebindMeshPacket(runtimeUUID,
            cellID,
            //m_cell->getCellID(),
            m_uuid,
            dstCellID,
            0,
            type,
            m_uuid,
            m_cellID,
            m_cellID,
            nullPoint,
            nullPoint,
            discoverySAP,
            meshSAP,
            ftSAP,
            mb);
    RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
    delete packet;
    if (request == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:rebindMesh() - Request failed\n")));
        return 0;
    }
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(JOIN_CELL_TIMEOUT_MS);
    timeout += delta;
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafClientHandler:rebindMesh() After future\n")));
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
    if (results != 0 && results->size() > 0) {
        RebindMeshReplyPacket* replyPacket = static_cast<RebindMeshReplyPacket*> (results->front());
        bool status = replyPacket->getJoinResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        if (m_debugLeafClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler::rebindMesh() - Rebind (%d)\n"), status));
        }
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::rebindMesh() - No response\n")));
    }

    return false;

}

bool LeafClientHandler::updateInfo(InfoUpdatePtr& updateInfoPtr) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    SthenoPacket *packet = 0;
    UUIDPtr srcCellID(new CellID(m_cellID.get()));
    UUIDPtr dstCellID(new CellID(m_cellID.get()));
    StreamSize ss;
    updateInfoPtr->serialize(ss);
    ByteOutputStream os(ss.total_length());
    updateInfoPtr->serialize(os);
    ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
    RDR::consolidate(mb, &os.start_);
    UUIDPtr runtimeUUID;
    m_mesh->getUUID(runtimeUUID);
    UUIDPtr fid;
    m_mesh->getFID(fid);
    packet = new UpdateInfoPacket(runtimeUUID,
            srcCellID,
            m_uuid,
            dstCellID,
            0,
            mb);
    RequestEngine<SthenoPacket*>::RequestPtr* request = sendRequest(packet, 0);
    delete packet;
    if (request == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:updateInfo() - Request failed\n")));
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Before wait future\n");
    ACE_Time_Value timeout = ACE_OS::gettimeofday();
    ACE_Time_Value delta;
    delta.msec(JOIN_CELL_TIMEOUT_MS);
    timeout += delta;
    list<SthenoPacket*>* results = request->get()->waitFuture(0);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)After future\n");
    if (results != 0 && results->size() > 0) {
        UpdateInfoReplyPacket* replyPacket = static_cast<UpdateInfoReplyPacket*> (results->front());
        bool status = replyPacket->getUpdateResult();
        ListHelper<SthenoPacket*>::deleteList(results);
        return status;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::updateInfo() - No response\n")));
    }

    return false;
}

int LeafClientHandler::handlePacket(SthenoPacket* packet) {
    switch (packet->getPacketHeader()->getType()) {
        case JOIN_MESH_REPLY_PACKET:
        {
            JoinMeshReplyPacket* joinMeshReplyPacket = static_cast<JoinMeshReplyPacket*> (packet);
            UInt id = joinMeshReplyPacket->getRequestID();
            if (m_requests.setReply(id, packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::handlePacket() - request is gone?(%d)\n"),
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
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::handlePacket() - request is gone?(%d)\n"),
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
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::handlePacket() - request is gone?(%d)\n"),
                        packet->getPacketHeader()->getType()));
                delete packet;
                return -1;
            }
            return 0;
        }

        case UPDATEINFO_MESH_REPLY_PACKET://UPDATEINFO_MESH_PACKET:
        {
            UpdateInfoReplyPacket* updateInfoReplyPacket = static_cast<UpdateInfoReplyPacket*> (packet);
            UInt id = updateInfoReplyPacket->getReplyPacketID();
            if (m_requests.setReply(id, packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::handlePacket() - request is gone?(%d)\n"),
                        packet->getPacketHeader()->getType()));
                delete packet;
                return -1;
            }
            return 0;
        }

        default:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler::handlePacket() we don't know how to handle(%d)\n"),
                    packet->getPacketHeader()->getType()));
            delete packet;
            return -1;
    }


}

RequestEngine<SthenoPacket*>::RequestPtr* LeafClientHandler::sendRequest(SthenoPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    if (m_close) {
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    if (m_debugLeafClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafClientHandler:sendRequest(): buffer_size=%d packet_serialize_size=%d\n"),
                mb->length(), packet->getSerializationSize()));
    }
    UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
    RequestEngine<SthenoPacket*>::RequestPtr* request =
            new RequestEngine<SthenoPacket*>::RequestPtr(new Request<SthenoPacket*>(&m_requests, id, timeout));
    m_requests.bind(request);
    int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
    ACE_Message_Block::release(mb);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafClientHandler:sendRequest() - register ID(%d) failed!"), id));
        delete request;
        return 0;
    }
    return request;
}
