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
 * File:   ReplicaGroup.cpp
 * Author: rmartins
 * 
 * Created on August 4, 2010, 3:45 PM
 */

#include "ReplicationGroup.h"
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/SthenoCoreInterface.h>

ReplicationGroup::ReplicationGroup(
        FaultTolerance* ft,
        UUIDPtr& rgid,
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        QoSManagerInterface* qosManager) throw (ServiceException&) :
m_ft(ft),
m_closed(false),
m_svcPtr(svcPtr),
//m_sid(m_svcPtr->getSID()),
m_sid(0),
m_params(params),
//generate a new rgid
//m_groupUUID(UUID::generateUUID()),
m_groupUUID(rgid),
m_qosManager(qosManager) {
    m_svcPtr->getSID(m_sid),
    m_primary = true;
}

ReplicationGroup::ReplicationGroup(
        FaultTolerance* ft,
        UUIDPtr& rgid,
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        OverlayPeerInfoPtr& primaryPeer,
        list<OverlayPeerInfoPtr>& replicas,
        QoSManagerInterface* qosManager) throw (ServiceException&) :
m_ft(ft),
m_closed(false),
m_svcPtr(svcPtr),
//m_sid(m_svcPtr->getSID()),
        m_sid(0),
m_primaryPeer(primaryPeer),
m_replicas(replicas),
m_params(params),
m_groupUUID(rgid),
m_qosManager(qosManager) {
    m_svcPtr->getSID(m_sid);
    m_primary = false;
}

ReplicationGroup::~ReplicationGroup() {
    if (!m_closed) {
        close();
    }
}

void ReplicationGroup::close() {
    m_closed = true;

}

QoSManagerInterface* ReplicationGroup::getQoSManager() {
    return m_qosManager;
}

UUIDPtr& ReplicationGroup::getGroupUUID() {
    return m_groupUUID;
}

bool ReplicationGroup::isThisPeer(UUIDPtr& uuid) {
    UUIDPtr runtimeUUID;
    m_ft->getOverlay()->getUUID(runtimeUUID);
    return runtimeUUID->isEqual(uuid);
}

int ReplicationGroup::isReplicaPrecedent(UUIDPtr& uuid) {
    int ourIndex = -1;
    int replicaIndex = -1;
    int i = 0;
    list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    while (iter != m_replicas.end()) {
        if (isThisPeer((*iter)->getUUID())) {
            ourIndex = i;
        }
        if ((*iter)->getUUID()->isEqual(uuid)) {
            replicaIndex = i;
        }
        if (ourIndex != -1 && replicaIndex != -1) {
            break;
        }
        i++;
        iter++;
    }
    if (replicaIndex == -1) {
        return -1;
    }
    if (replicaIndex < ourIndex) {
        return 0;
    } else {
        return 1;
    }
}

bool ReplicationGroup::isPrimary() {
    return m_primary;
}

bool ReplicationGroup::addReplica(OverlayPeerInfoPtr& replica) {
    //Update the group view!
    //do it before propagating it!
    //m_peers->push_back(replica);
    //m_bindedPeers.push_back(replica);
    //NOT HERE
    if (isPrimary()) {
        return addReplicaAsPrimary(replica);
    } else {
        return addReplicaAsReplica(replica);
    }
}

ServiceAbstractPtr& ReplicationGroup::getService() {
    return m_svcPtr;
}

OverlayPeerInfoPtr& ReplicationGroup::getPrimary() {
    return m_primaryPeer;
}

list<OverlayPeerInfoPtr>& ReplicationGroup::getReplicaList() {
    return m_replicas;
}

void ReplicationGroup::getSID(UUIDPtr& sid) {
    sid = m_sid;
}

ServiceParamsPtr& ReplicationGroup::getServiceParams() {
    return m_params;
}

FaultTolerance* ReplicationGroup::getFaultToleranceService() {
    return m_ft;
}
