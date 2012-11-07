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
 * File:   FTParticipant.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on August 6, 2010, 11:51 AM
 */

#include "FTParticipant.h"
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/core/p2p/ft/ReplicationGroup.h>

FTParticipant::FTParticipant(ReplicationGroup* replicaGroup,
        UUIDPtr& peerID,
        UUIDPtr& sid,
        UUIDPtr& iid,
        QoSReservation* reserve,
        FTReplicaParams* params) :
m_replicaGroup(replicaGroup), m_peerID(peerID), m_sid(sid), m_iid(iid),
m_reserve(reserve), m_params(params) {
}

FTParticipant::~FTParticipant() {
    if (m_reserve != 0) {
        delete m_reserve;
    }
    if (m_params != 0) {
        delete m_params;
    }
}

void FTParticipant::open() {
    //m_replicaGroup->startReplica(m_peerID,m_sid,m_iid,m_reserve,m_params);
}

void FTParticipant::asyncOpen(AsyncStart& startToken) {
    //m_replicaGroup->asyncStartReplica(startToken,m_peerID,m_sid,m_iid,m_reserve,m_params);
}

void FTParticipant::close() {
}

UUIDPtr& FTParticipant::getPeerID() {
    return m_peerID;
}

UUIDPtr& FTParticipant::getSID() {
    return m_peerID;
}

UUIDPtr& FTParticipant::getIID() {
    return m_peerID;
}

QoSReservation* FTParticipant::getQoSReservation() {
    return m_reserve;
}

FTReplicaParams* FTParticipant::getFTReplicaParams() {
    return m_params;
}
