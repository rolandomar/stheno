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
 * File:   Primary.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on August 6, 2010, 11:50 AM
 */

#ifndef PRIMARY_H
#define	PRIMARY_H

#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceAbstract.h>
#include <stheno/service/FTReplicaParams.h>
#include <stheno/core/p2p/ft/AsyncStart.h>
#include <stheno/core/p2p/ft/FTParticipant.h>

class Primary: public FTParticipant {
public:
    Primary(ReplicationGroup* replicaGroup,
            UUIDPtr& peerID,
            UUIDPtr& sid,
            UUIDPtr& iid,
            QoSReservation* reserve,
            FTReplicaParams* params);
    virtual ~Primary();

protected:
};

#endif	/* PRIMARY_H */

