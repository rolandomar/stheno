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
 * File:   Primary.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on August 6, 2010, 11:50 AM
 */

#include "Primary.h"
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/core/p2p/ft/ReplicationGroup.h>

Primary::Primary(ReplicationGroup* replicaGroup,
        UUIDPtr& peerID,
        UUIDPtr& sid,
        UUIDPtr& iid,
        QoSReservation* reserve,
        FTReplicaParams* params):
        FTParticipant(replicaGroup,peerID,sid,iid,reserve,params)

{
}

Primary::~Primary() {
}

