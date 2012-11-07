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
 * File:   FTDataClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:29 AM
 */

#include "FTDataClientHandler.h"
#include "packet/FTInitDataGroupReplyPacket.h"
#include "packet/FTDataPrebindReplyPacket.h"
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/mesh/Mesh.h>

FTDataClientHandler::~FTDataClientHandler() {
    /*if (m_requests != 0) {
        printf("FTDataClientHandler:~FTDataClientHandler() - SIZE=%lu\n", m_requests->getSize());
        delete m_requests;
    }*/
    printf("FTDataClientHandler::~FTDataClientHandler()\n");
    close();
}

FTDataClientHandler::FTDataClientHandler() {
    m_debugFTDataClientHandler = RuntimeConfiguration::isClassDebugged("FTDataClientHandler");
    throw -1;
}

FTDataClientHandler::FTDataClientHandler(P3ReplicationGroup* rg, UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& rgid,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new FTDataPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener),
m_rg(rg),
m_uuid(uuid),
m_fid(fid),
m_rgid(rgid) {
    m_debugFTDataClientHandler = RuntimeConfiguration::isClassDebugged("FTDataClientHandler");
    m_requests = new ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>();
}

int FTDataClientHandler::initSession() {
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
    FTInitDataGroupPacket* initPacket = new FTInitDataGroupPacket(
            0,
            runtimeUUID,
            fid,
            m_rgid);
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("\n\n\n(%t|%T)INFO: FTDataClientHandler::INFO initSession() - before send init packet - PID=%s\n\n\n"),
                m_uuid->toString().c_str()));
    }
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, 0);
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTDataClientHandler::INFO initSession() - after send init packet, ret=%d\n"), ret));
    }
    if (ret == -1) {
        return -1;
    }
    SthenoStreamPacket* receivePacket = 0;
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTDataClientHandler::INFO initSession() - before recv init reply packet\n")));
    }
    ret = this->receivePacket(receivePacket, 0);
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)FTDataClientHandler::INFO initSession() - after recv init reply packet ret =%d\n"), ret));
    }
    if (receivePacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler::initSession() packet null\n")));
        return -1;
    }
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::initSession() - got answer (%d)\n"), ret));
    }
    if (receivePacket->getPacketHeader()->getType() != FTInitDataGroupReplyPacket::INIT_DATA_GROUP_REPLY_PACKET_OP) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler:: packet incorret(%d)\n"), receivePacket->getPacketHeader()->getType()));
        return -1;
    }

    //if (m_debugFTDataClientHandler) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::initSession() - done\n")));
    //}

    return ret;
}

int FTDataClientHandler::preBindSession() {
    MeshPtr meshPtr;
    try {
        m_rg->getFaultToleranceService()->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        return -1;
    }

    UUIDPtr runtimeUUID;
    m_rg->getFaultToleranceService()->getOverlay()->getUUID(runtimeUUID);
    UUIDPtr fid;
    meshPtr->getFID(fid);
    FTDataPrebindPacket* initPacket = new FTDataPrebindPacket(
            0,
            runtimeUUID,
            fid,
            m_rgid);
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)(%t|%T)INFO: FTDataClientHandler::preBindSession() - before send init packet\n")));
    }
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, 0);

    delete initPacket;

    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)(%t|%T)INFO: FTDataClientHandler::preBindSession() - after send init packet, ret=%d\n"), ret));
    }
    if (ret == -1) {
        return -1;
    }
    SthenoStreamPacket* receivePacket = 0;
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::preBindSession() - before recv init reply packet\n")));
    }
    ret = this->receivePacket(receivePacket, 0);
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::preBindSession() - after recv init reply packet ret =%d\n"), ret));
    }
    if (receivePacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler::preBindSession() packet null\n")));
        return -1;
    }
    if (m_debugFTDataClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::preBindSession() - got answer\n")));
    }
    if (receivePacket->getPacketHeader()->getType() != FTDataPrebindReplyPacket::FT_DATA_PREBIND_REPLY_PACKET_OP) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler::preBindSession() packet incorret(%d)\n"), receivePacket->getPacketHeader()->getType()));
        delete receivePacket;
        return -1;
    }
    //if (m_debugFTDataClientHandler) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler::preBindSession() - done\n")));
    //}
    delete receivePacket;
    return ret;
}

ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr* FTDataClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
    if (m_close) {
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<FTDataClientHandler, SthenoStreamPacket*>(this, m_requests, id, timeout));
    m_requests->bind(request);
    int ret = 0;
    if (this->hasAsyncWrite()) {
        WriteBlockPtr* wb = new WriteBlockPtr(new WriteBlock(mb, timeout));
        WriteBlockPtr* wbAsync = new WriteBlockPtr(*wb);
        (*request)->attachWriteBlock(wb);
        ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::sendAsync(wbAsync);
    } else {
        ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler:sendRequest() - ID(%d)\n"), id));
        delete request;
        return 0;
    }
    return request;
}

/*ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr* FTDataClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<FTDataClientHandler, SthenoStreamPacket*>(this, m_requests, id, timeout));
    m_requests->bind(request);    
    int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
    ACE_Message_Block::release(mb);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler:sendRequest() - ID(%d)\n"), id));
        delete request;
        return 0;
    }
    return request;
}*/