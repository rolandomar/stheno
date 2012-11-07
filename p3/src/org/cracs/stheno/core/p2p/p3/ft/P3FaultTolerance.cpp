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
 * File:   P3FaultTolerance.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 24, 2010, 4:06 PM
 */

#include "P3FaultTolerance.h"

#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/p2p/discovery/DiscoveryInterface.h>
#include <stheno/core/p2p/OverlayInterface.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQuery.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQueryReply.h>
#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

P3FaultTolerance::P3FaultTolerance(Overlay* overlay) : FaultTolerance(overlay),
m_controlSAP(this) {
}

P3FaultTolerance::~P3FaultTolerance() {
}

QoSResources* P3FaultTolerance::calculateQoSResources(ServiceParamsPtr& params) {
    return 0;
}

ServiceAbstract* P3FaultTolerance::duplicate() throw (ServiceException&) {
    return 0;
}

const char* P3FaultTolerance::getName() {
    return "P3FaultTolerance";
}

void P3FaultTolerance::getSAP(SAPInfoPtr& sapInfo) {
    sapInfo = m_controlSAP.getSAPInfo();
}

const char* P3FaultTolerance::id() {
    return "CAB2C3FD0BE5CFC5CD14EF63458E702D";
}

void P3FaultTolerance::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    /*String domainPath = "runtime_" + this->getOverlay()->getUUID()->toString();
    domainPath += "/p2p/ft";
    CPUGroupQoS qos("/HRT", domainPath, 10000, QoSManagerInterface::DEFAULT_PERIOD, 1024);
    this->getOverlay()->getRuntime()->getQoSManager()->createGroupCPUReserve(domainPath, qos);*/
    list<QoSEndpoint> qosList;
    //QoSEndpoint hrt(QoS::HRT, CPUQoS::MED_RT_PRIO);
    QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    qosList.push_back(hrt);
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3FaultTolerance::open_i(%d) creating SAP... exception!\n"), m_status));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    m_controlSAP.open(itf, qosList);
}

list<OverlayPeerInfoPtr>* P3FaultTolerance::findResources(UUIDPtr& sid,
        ServiceParamsPtr& params,
        UInt replicaCount) {
    //UInt replicaCount = params->getFTParams()->getReplicaCount();
    list<OverlayPeerInfoPtr>* peerList = new list<OverlayPeerInfoPtr > ();
    for (int i = 0; i < replicaCount; i++) {
        list<UUIDPtr> filterPeers;
        getFilterPeers(peerList, filterPeers);
        UUIDPtr runtimeUUID;
        this->getOverlay()->getUUID(runtimeUUID);
        LaunchServiceSiteQuery* query = new LaunchServiceSiteQuery(runtimeUUID, sid, filterPeers);
        DiscoveryPtr discoveryPtr;
        try {
            m_overlay->getDiscovery(discoveryPtr);
        } catch (OverlayException& ex) {
            return 0;
            //throw 
        }
        DiscoveryQueryReply* reply = discoveryPtr->executeQuery(query);
        if(reply == 0){
            //no resources are available!
            return 0;
        }
        //DiscoveryQueryReply* reply = m_overlay->getDiscovery()->executeQuery(query);
        LaunchServiceSiteQueryReply* replyNarrow = 0;
        if (reply->getQuerySpecializationBlob() == 0) {
            replyNarrow = static_cast<LaunchServiceSiteQueryReply*> (reply);
        } else {
            replyNarrow = new LaunchServiceSiteQueryReply(reply);
        }
        printf("FaultTolerance::createReplicationGroup: PoL UUID=%s FID=%s SAP=%p SAP=%p\n",
                replyNarrow->getUUID()->toString().c_str(),
                replyNarrow->getFID()->toString().c_str(),
                replyNarrow->getMeshSAPInfo().get(),
                replyNarrow->getFTSAPInfo().get()
                );

        Endpoint e;
        replyNarrow->getFTSAPInfo()->getFirstEndpoint(e);
        printf("FaultTolerance::createReplicationGroup: ENDPONIT=%s\n", e.toString().c_str());

        OverlayPeerInfo* pi = new OverlayPeerInfo(replyNarrow->getUUID(), replyNarrow->getFID(),
                replyNarrow->getMeshSAPInfo(), replyNarrow->getFTSAPInfo());
        OverlayPeerInfoPtr piPtr(pi);
        peerList->push_back(piPtr);
        delete replyNarrow;
    }
    //return 0;
    return peerList;
}

void P3FaultTolerance::getFilterPeers(list<OverlayPeerInfoPtr>* replicas, list<UUIDPtr>& filterPeers) {
    list<OverlayPeerInfoPtr>::iterator iter = replicas->begin();
    while (iter != replicas->end()) {
        filterPeers.push_back((*iter)->getUUID());
        iter++;
    }
}

void P3FaultTolerance::createReplicationGroup(
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        UUIDPtr& rgid,
        QoSManagerInterface* qosManager
        ) throw (ServiceException&) {
    //QoSResources* qos = svcPtr->calculateQoSResources(params);
    //list<OverlayPeerInfoPtr>* peers = findResources(svcPtr, params);    
    //Replica group
    //P3ReplicationGroup* group = (P3ReplicationGroup*) createReplicaGroup(peers, svcPtr, params,rgid);
    //TODO: change this    
    P3ReplicationGroup* group = new P3ReplicationGroup(this, rgid, svcPtr, params, qosManager);
    //P3ReplicationGroupPtr groupPtr(group);
    ReplicationGroupPtr groupPtr(group);
    m_replicationGroupManager.addReplicationGroup(groupPtr);
    try {
        group->open();
    } catch (ServiceException& ex) {
        //delete group;
        m_replicationGroupManager.removeReplicationGroup(groupPtr->getGroupUUID());
        throw ex;
    }

}

void P3FaultTolerance::createReplicationGroup(
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        UUIDPtr& rgid,
        OverlayPeerInfoPtr& primary,
        list<OverlayPeerInfoPtr>& replicas,
        QoSManagerInterface* qosManager
        ) throw (ServiceException&) {
    //QoSResources* qos = svcPtr->calculateQoSResources(params);
    //list<OverlayPeerInfoPtr>* peers = findResources(svcPtr, params);    
    //Replica group
    //P3ReplicationGroup* group = (P3ReplicationGroup*) createReplicaGroup(peers, svcPtr, params,rgid);
    //TODO: change this    
    P3ReplicationGroup* group = new P3ReplicationGroup(this, rgid, svcPtr, params, primary, replicas, qosManager);
    ReplicationGroupPtr groupPtr(group);
    m_replicationGroupManager.addReplicationGroup(groupPtr);
    try {
        group->open();
    } catch (ServiceException& ex) {
        //delete group;
        m_replicationGroupManager.removeReplicationGroup(groupPtr->getGroupUUID());
        throw ex;
    }


    /*Replica group
    //P3ReplicationGroup* group = (P3ReplicationGroup*) createReplicaGroup(peers, rgid,sid, params);    
    P3ReplicationGroup* group = new P3ReplicationGroup(this, primary, replicas, rgid, sid, params, qosManager);
    rgid = group->getGroupUUID();
    P3ReplicationGroupPtr groupPtr(group);
    m_replicationGroupManager.addReplicationGroup(groupPtr);
    try {
        group->open();
    } catch (ServiceException& ex) {
        //delete group;
        m_replicationGroupManager.removeReplicationGroup(groupPtr->getGroupUUID());
        throw ex;
    }*/

}

void P3FaultTolerance::onReplicationGroupClose(UUIDPtr& rgid) {
    m_replicationGroupManager.removeReplicationGroup(rgid);
}

bool P3FaultTolerance::getReplicationGroup(UUIDPtr& rgid, ReplicationGroupPtr& rgPtr) {
    return m_replicationGroupManager.getReplicationGroup(rgid, rgPtr);
}
