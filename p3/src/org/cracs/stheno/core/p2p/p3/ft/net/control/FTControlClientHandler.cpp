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
 * File:   FTControlClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:29 AM
 */

#include "FTControlClientHandler.h"
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/mesh/Mesh.h>

FTControlClientHandler::~FTControlClientHandler() {
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n\n\n(%t|%T)INFO: FTControlClientHandler::~FTControlClientHandler %@\n"),
                this));
    }
    /*if(m_requests != 0){
        delete m_requests;
    }*/
    //printf("FTControlClientHandler::~FTControlClientHandler()\n");
    close();
    //printf("FTControlClientHandler::~FTControlClientHandler() end\n");
}

FTControlClientHandler::FTControlClientHandler() {
    m_debugFTControlClientHandler = RuntimeConfiguration::isClassDebugged("FTControlClientHandler");
    throw -1;
}

FTControlClientHandler::FTControlClientHandler(P3ReplicationGroup* rg, UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& rgid,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new FTControlPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_rg(rg), m_uuid(uuid), m_fid(fid), m_rgid(rgid) {
    m_debugFTControlClientHandler = RuntimeConfiguration::isClassDebugged("FTControlClientHandler");
    //m_requests = new ChannelRequestEngine<FTControlClientHandler, SthenoStreamPacket*>();
}

int FTControlClientHandler::open(void *arg) {
    //FTInitReplicaPacket* initPacket = new FTInitReplicaPacket();
    //InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qos);
    m_receiveSequence = 0;
    /*StartStreamServicePacket* initPacket = new StartStreamServicePacket(0, m_uuid, m_fid);
    ACE_Time_Value* timeout = 0;
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
    if (ret == -1) {
        return -1;
    }
    ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
    return ret;*/
    return 0;
}

int FTControlClientHandler::initSession(
        UUIDPtr& sid,
        UUIDPtr& iid,
        ServiceParamsPtr& params,
        OverlayPeerInfoPtr& primary,
        list<OverlayPeerInfoPtr>& group,
        SAPInfoPtr& ftDataSAPInfo) {

    MeshPtr meshPtr;
    try {
        m_rg->getFaultToleranceService()->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        return -1;
    }
    //P3Mesh* mesh = static_cast<P3Mesh*> (meshPtr.get());
    UUIDPtr runtimeUUID;
    UUIDPtr fid;
    m_rg->getFaultToleranceService()->getOverlay()->getUUID(runtimeUUID);
    meshPtr->getFID(fid);
    FTInitReplicaPacket* initPacket = new FTInitReplicaPacket(
            0,
            runtimeUUID,
            fid,
            m_rgid, sid, iid, params, primary, group);
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n\n\n(%t|%T)INFO: FTControlClientHandler::INFO initSession() - before send init packet - PID=%s\n\n\n"),
                m_uuid->toString().c_str()));
    }
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, 0);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTControlClientHandler::INFO initSession() - after send init packet, ret=%d\n"), ret));
    if (ret == -1) {
        return -1;
    }
    SthenoStreamPacket* receivePacket = 0;
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTControlClientHandler::INFO initSession() - before recv init reply packet\n")));
    }
    ret = this->receivePacket(receivePacket, 0);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTControlClientHandler::INFO initSession() - after recv init reply packet ret =%d\n"), ret));
    if (receivePacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler::initSession() packet null\n")));
        return -1;
    }
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::initSession() - got answer (%d)\n"), ret));
    }
    if (receivePacket->getPacketHeader()->getType() != FTInitReplicaReplyPacket::INIT_REPLICA_REPLY_PACKET_OP) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler:: packet incorret(%d)\n"), receivePacket->getPacketHeader()->getType()));
        return -1;
    }
    ftDataSAPInfo = static_cast<FTInitReplicaReplyPacket*> (receivePacket)->getFTDataSAPInfo();
    //if (m_debugFTControlClientHandler) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::initSession() - OK dataSAP is %@\n"), ftDataSAPInfo.get()));
    //}
    return ret;
}

int FTControlClientHandler::preBindSession() {
    MeshPtr meshPtr;
    try {
        m_rg->getFaultToleranceService()->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        return -1;
    }
    UUIDPtr runtimeUUID;
    UUIDPtr fid;
    m_rg->getFaultToleranceService()->getOverlay()->getUUID(runtimeUUID);
    meshPtr->getFID(fid);
    FTPrebindPacket* initPacket = new FTPrebindPacket(
            0,
            runtimeUUID,
            fid,
            m_rgid);
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - before send init packet\n")));
    }
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, 0);
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - after send init packet, ret=%d\n"), ret));
    }
    if (ret == -1) {
        return -1;
    }
    SthenoStreamPacket* receivePacket = 0;
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - before recv init reply packet\n")));
    }
    ret = this->receivePacket(receivePacket, 0);
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - after recv init reply packet ret =%d\n"), ret));
    }
    if (receivePacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler::preBindSession() packet null\n")));
        return -1;
    }
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - got answer\n")));
    }
    if (receivePacket->getPacketHeader()->getType() != FTPrebindReplyPacket::PREBIND_REPLY_PACKET_OP) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler::preBindSession() packet incorret(%d)\n"), receivePacket->getPacketHeader()->getType()));
        return -1;
    }
    //if (m_debugFTControlClientHandler) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler::preBindSession() - done\n")));
    //}
    return ret;
}

int FTControlClientHandler::handlePacket(SthenoStreamPacket* packet) {
    if (m_debugFTControlClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_fid->toString().c_str()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case FTAddReplicaReplyPacket::ADD_REPLICA_REPLY_PACKET_OP:
        {
            if (m_debugFTControlClientHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler:handlePacket() - ADD_REPLICA_REPLY_PACKET_OP ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
            }
            FTAddReplicaReplyPacket* replyPacket = static_cast<FTAddReplicaReplyPacket*> (packet);
            if (m_requests.setReply(replyPacket->getPacketReplyID(), packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler:handlePacket() - ADD_REPLICA_REPLY_PACKET_OP failed to set reply ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
                delete packet;
                return -1;
            }
            return 0;
        }
        case FTRemoveReplicaReplyPacket::REMOVE_REPLICA_REPLY_PACKET_OP:
        {
            if (m_debugFTControlClientHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler:handlePacket() - REMOVE_REPLICA_REPLY_PACKET_OP ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
            }
            FTRemoveReplicaReplyPacket* replyPacket = static_cast<FTRemoveReplicaReplyPacket*> (packet);
            if (m_requests.setReply(replyPacket->getPacketReplyID(), packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlClientHandler:handlePacket() - REMOVE_REPLICA_REPLY_PACKET_OP failed to set reply ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
                delete packet;
                return -1;
            }
            return 0;
        }
        default:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
            delete packet;

            return 0;
    }
    return 0;
}

ACE_Reactor* FTControlClientHandler::reactor(void) const {
    ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));

    return reactor;
}

void FTControlClientHandler::reactor(ACE_Reactor *reactor) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));

    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
}

UUIDPtr& FTControlClientHandler::getPID() {

    return m_uuid;
}

UUIDPtr& FTControlClientHandler::getFID() {

    return m_fid;
}

UUIDPtr& FTControlClientHandler::getRGID() {
    return m_rgid;
}
