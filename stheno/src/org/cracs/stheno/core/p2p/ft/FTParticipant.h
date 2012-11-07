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
 * File:   FTParticipant.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on August 6, 2010, 11:51 AM
 */

#ifndef FTPARTICIPANT_H
#define	FTPARTICIPANT_H

#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceAbstract.h>
#include <stheno/service/FTReplicaParams.h>
#include <stheno/core/p2p/ft/AsyncStart.h>

class ReplicationGroup;
class FTParticipant {
public:
    FTParticipant(ReplicationGroup* replicaGroup,
            UUIDPtr& peerID,
            UUIDPtr& sid,
            UUIDPtr& iid,
            QoSReservation* reserve,
            FTReplicaParams* params);
    virtual ~FTParticipant();

    void asyncOpen(AsyncStart& startToken);
    void open();
    void close();

    UUIDPtr& getPeerID();
    UUIDPtr& getSID();
    UUIDPtr& getIID();
    QoSReservation* getQoSReservation();
    FTReplicaParams* getFTReplicaParams();

protected:
    ReplicationGroup* m_replicaGroup;
    UUIDPtr m_peerID;
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    QoSReservation* m_reserve;
    FTReplicaParams* m_params;
};

#endif	/* FTPARTICIPANT_H */

