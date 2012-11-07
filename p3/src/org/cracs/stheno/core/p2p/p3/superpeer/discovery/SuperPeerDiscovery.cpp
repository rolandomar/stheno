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
 * File:   SuperPeerDiscovery.cpp
 * Author: rmartins
 * 
 * Created on October 14, 2010, 5:49 PM
 */

#include "SuperPeerDiscovery.h"
#include <stheno/core/p2p/p3/superpeer/SuperPeer.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3GeneralProvider.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQuery.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQueryReply.h>
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
#include <stheno/core/p2p/p3/P3.h>
#include <euryale/net/RequestHandler.h>

#include <stheno/core/p2p/network/net/stream/StreamChannel.h>

#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

#include <stheno/common/TraceRuntime.h>

SuperPeerDiscovery::SuperPeerDiscovery(P3* overlay) : P3Discovery(overlay) {
    m_debugSuperPeerDiscovery = RuntimeConfiguration::isClassDebugged("SuperPeerDiscovery");
}

SuperPeerDiscovery::~SuperPeerDiscovery() {
}

SuperPeer* SuperPeerDiscovery::getOverlay_i() {
    return static_cast<SuperPeer*> (m_overlay);
}

/*void SuperPeerDiscovery::open(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    P3Discovery::open(params, ServiceAbstract::FT_NOT_SETTED);
    String domainPath = "runtime_" + this->getOverlay()->getUUID()->toString();
    domainPath += "/p2p/discovery";
    CPUGroupQoS qos("/HRT", domainPath, 10000, 300000, 1024);
    this->getOverlay()->getRuntime()->getQoSManager()->createGroupCPUReserve(domainPath, qos);
    P3GeneralProvider* p = new P3GeneralProvider(this);
    this->addDiscoveryProvider(p);
}*/
void SuperPeerDiscovery::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    P3Discovery::open_i(params, ServiceAbstract::FT_NOT_SETTED);
    if (isStarting() /*|| isResuming()*/) {
        /*String domainPath = "runtime_" + this->getOverlay()->getUUID()->toString();
        domainPath += "/p2p/discovery";
        CPUGroupQoS qos("/HRT", domainPath, 10000, 300000, 1024);
        //this->getOverlay()->getRuntime()->getQoSManager()->createGroupCPUReserve(domainPath, qos);
        this->getQoSManager()->createGroupCPUReserve(domainPath, qos);*/


        P3GeneralProvider* p = new P3GeneralProvider(this);
        this->addDiscoveryProvider(p);
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::open(): error\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
}

void SuperPeerDiscovery::handleQueryPacket(P3DiscoverySvcHandler* handler, P3DiscoveryQueryPacket* packet) {
    if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::handleQueryPacket()\n")));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::handleQueryPacket() %d\n"), packet->getQuery()->getQuerySpecializationBlob()->total_length()));
    }
    DiscoveryQueryPtr query = packet->getQuery();
    //if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::handleQueryPacket() before execute %d\n"), query->getQuerySpecializationBlob()->total_length()));
    //}
    DiscoveryQueryReply* reply = executeQuery(query.get(), 0);
    if (reply == 0) {
        reply = new DiscoveryQueryReply(query->getProvider(), query->getType(), new DiscoveryException(DiscoveryException::NO_REPLY_AVAILABLE));
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::handleQueryPacket() after execution reply=%@\n"),reply));
    P3DiscoveryQueryReplyPacket* replyPacket =
            new P3DiscoveryQueryReplyPacket(0, packet->getPacketHeader()->getPacketID(),
            packet->getUUID(), packet->getFID(), reply);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::handleQueryPacket() after execution packet=%@\n"),replyPacket));
    handler->sendPacket(replyPacket, 0);
}

DiscoveryQueryReply* SuperPeerDiscovery::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    SuperPeer* overlay = getOverlay_i();
    UUIDPtr parentUUID;
    overlay->getP3MeshService()->getParentUUID(parentUUID);
    //if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery()\n")));
    //}
    if (parentUUID.null()) {
        //if (m_debugSuperPeerDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeerDiscovery::executeQuery() - no parent => Root, exec locally\n")));
        //}
        //are we root?
        //execute local and return
        return executeLocalQuery(query);
    }
    SAPInfoPtr discoverySAPInfo;
    overlay->getP3MeshService()->getDiscoverySAPInfo(parentUUID, discoverySAPInfo);
    if (discoverySAPInfo.null()) {
        //if (m_debugSuperPeerDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SuperPeerDiscovery::executeQuery() - no parent SAP\n")));
        //}
        return 0;
    }
    Endpoint endpoint;
    ACE_Connector<P3DiscoveryClientHandler, ACE_SOCK_Connector> connector;
    discoverySAPInfo->getFirstEndpoint(endpoint);
    QoSEndpoint qosE = *(endpoint.getQoS());
    /*if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() - send to coordinator/parent FID=%@\n"),
                overlay->getP3MeshService()->getFID().get()));
    }*/
    UUIDPtr runtimeUUID;
    overlay->getUUID(runtimeUUID);
    UUIDPtr fid;
    overlay->getP3MeshService()->getFID(fid);
    P3DiscoveryClientHandler* clientHandler = new P3DiscoveryClientHandler(
            runtimeUUID,
            fid,
            qosE,
            false, false, 0, 0, 0, 0);

    if (connector.connect(clientHandler, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: SuperPeerDiscovery::executeQuery() - connect failed:")));
        clientHandler->close();
        clientHandler = 0;
        delete clientHandler;
        return 0;
    } else {
        if (m_debugSuperPeerDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() - Connect OK!\n")));
        }

    }
    P3DiscoveryQueryPacket* queryPacket = new P3DiscoveryQueryPacket(0, runtimeUUID,
            fid, qosE, query);
    clientHandler->sendPacket(static_cast<SthenoStreamPacket*> (queryPacket), 0);
    if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::sent query!\n")));
    }

    SthenoStreamPacket* packet = 0;
    if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() - send to coordinator/parent \n")));
    }
    clientHandler->receivePacket(packet, 0);
    if (packet == 0) {
        //if (m_debugSuperPeerDiscovery) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() - no reply\n")));
        //}
        return 0;
    }
    if (packet->getPacketHeader()->getType() != P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SuperPeerDiscovery::executeQuery() - wrong type of packet\n")));
        delete packet;
        return 0;
    }
    P3DiscoveryQueryReplyPacket* replyPacket = static_cast<P3DiscoveryQueryReplyPacket*> (packet);
    //DiscoveryQueryReplyPtr replyPtr = replyPacket->getQuery();
    DiscoveryQueryReply* reply = replyPacket->deAttachQuery();
    if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeQuery() reply=%@\n"),
                reply->getQuerySpecializationBlob()));
    }
    //DiscoveryQueryReply* reply = new DiscoveryQueryReply(0,0,0);
    //cleanup

    delete packet;
    clientHandler->close();
    delete clientHandler;
    ACE_Time_Value end = ACE_OS::gettimeofday();
    end -= start;
    TraceRuntimeSingleton::instance()->logQueryTime(query->getType(),end);
    return reply;
}

DiscoveryQueryReply* SuperPeerDiscovery::executeLocalQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    int type = query->getType();
    int providerID = query->getProvider();
    if (m_debugSuperPeerDiscovery) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery()\n")));
    }
    DiscoveryProviderInfo* providerInfo = this->findProvider(providerID);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - %@\n"),providerInfo));
    if (providerInfo != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - %@\n"),providerInfo->getProvider()));        
        DiscoveryQueryReply* reply = 0;
        try {
            reply = providerInfo->getProvider()->executeQuery(query, qos);
        } catch (DiscoveryException& ex) {
            //if (m_debugSuperPeerDiscovery) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - exception!.\n")));
            //}

            return 0;
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - after query\n"),providerInfo));
        if (reply != 0) {
            //if (m_debugSuperPeerDiscovery) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - provider replied.\n")));
            //}
            return reply;
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::executeLocalQuery() - provider couldn't provide reply.\n")));
        return 0;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SuperPeerDiscovery::executeLocalQuery() - no provider!\n")));
    return 0;
}



