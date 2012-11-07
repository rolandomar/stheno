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
 * File:   LeafDiscovery.cpp
 * Author: rmartins
 * 
 * Created on October 13, 2010, 11:35 PM
 */

#include "LeafDiscovery.h"
#include <stheno/core/p2p/p3/leaf/LeafPeer.h>
#include <stheno/core/p2p/p3/leaf/mesh/P3LeafMesh.h>
#include <stheno/core/p2p/p3/leaf/discovery/LeafDiscovery.h>

#include <stheno/common/TraceRuntime.h>

LeafDiscovery::LeafDiscovery(P3* overlay) : P3Discovery(overlay) {
    m_debugLeafDiscovery = RuntimeConfiguration::isClassDebugged("LeafDiscovery");
}

LeafDiscovery::~LeafDiscovery() {
}

LeafPeer* LeafDiscovery::getOverlay_i() {
    return static_cast<LeafPeer*> (m_overlay);
}

void LeafDiscovery::handleQueryPacket(P3DiscoverySvcHandler* handler, P3DiscoveryQueryPacket* packet) {
    delete packet;
    return;
}

/*void SuperPeerDiscovery::handleQueryReplyPacket(P3DiscoveryClientHandler* handler, P3DiscoveryQueryReplyPacket* packet) {
    delete packet;
    return;
}*/

DiscoveryQueryReply* LeafDiscovery::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    UInt queryBackup = query->getType();
    LeafPeer* overlay = getOverlay_i();
    UUIDPtr parentUUID;
    overlay->getP3MeshService()->getCoordinatorUUID(parentUUID);
    if (parentUUID.null()) {
        if (m_debugLeafDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() - no parent, exec locally\n")));
        }
        //are we root?
        //execute local and return
        return executeLocalQuery(query);
    }
    UUIDPtr runtimeUUID;
    overlay->getUUID(runtimeUUID);
    UUIDPtr uuid(runtimeUUID);
    UUIDPtr fid;
    overlay->getP3MeshService()->getFID(fid);
    if (fid.null()) {
        delete query;
        return 0;
    }

    SAPInfoPtr discoverySAPInfo;
    overlay->getP3MeshService()->getCoordinatorDiscoverySAP(discoverySAPInfo);
    if (discoverySAPInfo.null()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() - no discovery SAP\n")));
        delete query;
        return 0;
    }
    if (m_debugLeafDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery()...\n")));
    }
    Endpoint endpoint;
    ACE_Connector<P3DiscoveryClientHandler, ACE_SOCK_Connector> connector;
    try {
        discoverySAPInfo->getFirstEndpoint(endpoint);
    } catch (EndpointException& ex) {
        delete query;
        return 0;
    }
    QoSEndpoint qosE = *(endpoint.getQoS());
    if (m_debugLeafDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() 2...\n")));
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() 2,1 %s...\n"), overlay->getUUID()->toString().c_str()));
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() 2,2 %s...\n"), overlay->getP3MeshService()->getFID()->toString().c_str()));
    }
    P3DiscoveryClientHandler* clientHandler =
            new P3DiscoveryClientHandler(uuid,
            fid,
            qosE, false, false, 0, 0, 0, 0);
    if (m_debugLeafDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() 3...\n")));
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connecting...(%s)!\n"), endpoint.getAddr().toString().c_str()));
    if (connector.connect(clientHandler, endpoint.getAddr()) == -1) {
        /*ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)connect failed:")));*/
        clientHandler->close();
        delete clientHandler;
        clientHandler = 0;
        return 0;
        //throw ServiceException("Error connecting to coordinator!");
    } else {
        if (m_debugLeafDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() - Connect OK!\n")));
        }

    }

    P3DiscoveryQueryPacket* queryPacket = new P3DiscoveryQueryPacket(0, runtimeUUID,
            fid, qosE, query);

    clientHandler->sendPacket(static_cast<SthenoStreamPacket*> (queryPacket), 0);
    delete queryPacket;

    if (m_debugLeafDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::sent query!\n")));
    }

    SthenoStreamPacket* packet = 0;
    clientHandler->receivePacket(packet, 0);
    clientHandler->close();
    delete clientHandler;

    if (m_debugLeafDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::receivePacket query reply!\n")));
    }
    if (packet == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SuperPeerDiscovery::executeQuery() - no reply\n")));
        return 0;
    }
    if (packet->getPacketHeader()->getType() != P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SuperPeerDiscovery::executeQuery() - wrong type of packet\n")));
        delete packet;
        return 0;
    }
    P3DiscoveryQueryReplyPacket* replyPacket = static_cast<P3DiscoveryQueryReplyPacket*> (packet);
    DiscoveryQueryReply* reply = replyPacket->deAttachQuery();
    delete packet;
    ACE_Time_Value end = ACE_OS::gettimeofday();
    end -= start;
    //printf("\n\nTYpe=%d\n",queryBackup);
    TraceRuntimeSingleton::instance()->logQueryTime(queryBackup,end);
    return reply;
}

DiscoveryQueryReply* LeafDiscovery::executeLocalQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    if (query != 0) {
        delete query;
    }
    return 0;
}

AsyncDiscoveryQueryReply* LeafDiscovery::executeAsyncQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    return 0;
}