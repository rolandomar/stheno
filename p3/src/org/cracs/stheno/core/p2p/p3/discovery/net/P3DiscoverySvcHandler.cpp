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
 * File:   P3DiscoverySvcHandler.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 12:21 PM
 */

#include "P3DiscoverySvcHandler.h"
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryQueryPacket.h>
#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryQueryReplyPacket.h>

P3DiscoverySvcHandler::~P3DiscoverySvcHandler() {
    /*if (m_requests != 0) {
        delete m_requests;
        m_requests = 0;
    }*/
    close();
}

int P3DiscoverySvcHandler::handlePacket(SthenoStreamPacket* packet) {
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler:handlePacket() - ID=%d UUID=%s CELLID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_cellID->toString().c_str()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case P3DiscoveryQueryPacket::P3DISCOVERY_QUERY_PACKET:
        {
            m_discovery->handleQueryPacket(this, static_cast<P3DiscoveryQueryPacket*> (packet));
            return 0;
        }

            /*case P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET:{
            
            }*/
        default:
        {
            delete packet;
            return -1;
        }
    }
    return -1;
    //m_discovery->handlePacket(this,packet);
}

P3DiscoverySvcHandler::P3DiscoverySvcHandler(P3Discovery* discovery, //bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new P3DiscoveryPacketFactory(), false, false,
ec, reserve, qos, closeListener)/*, m_requests(0)*/ {
    m_debugP3DiscoverySvcHandler = RuntimeConfiguration::isClassDebugged("P3DiscoverySvcHandler");
    m_discovery = discovery;
    //m_requests = new RequestEngine<SthenoStreamPacket*>();
}

int P3DiscoverySvcHandler::open(void *arg) {
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: P3DiscoverySvcHandler::open\n")));
    }
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: P3DiscoverySvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoverySvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {//from children
            InitSessionPacket* initSessionPacket = static_cast<InitSessionPacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            if (m_debugP3DiscoverySvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
            }
            UUID* uuid = new UUID(*initSessionPacket->getFID().get());
            CellIDPtr cellID(new CellID(uuid));
            m_cellID = cellID;
            m_qosE = initSessionPacket->getQoS();
            if (m_debugP3DiscoverySvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler::open - OK PID=%s\n"), m_pid->toString().c_str()));
            }
            /*if (m_cell->handleJoinMesh(this, joinMeshPacket) != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - processJoinCell failde\n")));
                return -1;
            }*/
            return 0;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            return -1;
        }
    }
}

UUIDPtr& P3DiscoverySvcHandler::getPID() {
    return m_pid;
}

CellIDPtr& P3DiscoverySvcHandler::getCellID() {
    return m_cellID;
}

QoSEndpoint& P3DiscoverySvcHandler::getQoSEndpoint() {
    return m_qosE;
}


RequestEngine<SthenoStreamPacket*>::RequestPtr* P3DiscoverySvcHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_Message_Block* mb = serializePacket(packet);
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler:sendRequest() - ID=%d UUID=%s CELLID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_cellID->toString().c_str()));
    }
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    RequestEngine<SthenoStreamPacket*>::RequestPtr* request =
            new RequestEngine<SthenoStreamPacket*>::RequestPtr(new Request<SthenoStreamPacket*>(&m_requests, id, timeout));
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - before bind PeerID=%s ID=%d\n"),
                this->getPID()->toString().c_str(),
                packet->getPacketHeader()->getPacketID()));
    }
    m_requests.bind(request);
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - after bind PeerID=%s ID=%d\n"),
                this->getPID()->toString().c_str(),
                packet->getPacketHeader()->getPacketID()));
    }
    int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
    ACE_Message_Block::release(mb);
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - sent PeerID=%s ID=%d\n"),
                this->getPID()->toString().c_str(),
                packet->getPacketHeader()->getPacketID()));
    }
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR:: P3DiscoverySvcHandler::sendRequest() - send PeerID=%s ID(%d) failed!\n"),
                this->getPID()->toString().c_str(),
                id));
        delete request;
        return 0;
    }
    if (m_debugP3DiscoverySvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - END PeerID=%s ID=%d\n"),
                this->getPID()->toString().c_str(),
                packet->getPacketHeader()->getPacketID()));
    }
    return request;
}
