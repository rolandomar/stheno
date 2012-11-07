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
 * File:   Replica.h
 * Author: rmartins
 *
 * Created on August 5, 2010, 11:19 AM
 */

#ifndef REPLICA_H
#define	REPLICA_H
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceAbstract.h>
#include <stheno/service/FTReplicaParams.h>
#include <stheno/core/p2p/ft/AsyncStart.h>
#include <stheno/core/p2p/ft/FTParticipant.h>

class Replica: public FTParticipant {
public:    
    Replica(ReplicationGroup* replicaGroup,
            UUIDPtr& peerID, 
            UUIDPtr& sid, 
            UUIDPtr& iid,             
            QoSReservation* reserve,
            FTReplicaParams* params);
    virtual ~Replica();
        
protected:    
};

#endif	/* REPLICA_H */

