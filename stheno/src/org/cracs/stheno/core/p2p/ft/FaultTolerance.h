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
 * File:   FaultTolerance.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 21, 2010, 11:14 AM
 */

#ifndef FAULTTOLERANCE_H
#define	FAULTTOLERANCE_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <stheno/core/p2p/P2PService.h>

#include <euryale/common/uuid/UUID.h>
#include <euryale/common/uuid/PeerLocation.h>
#include <stheno/core/p2p/detection/FaultDetection.h>
#include <stheno/service/FTReplicaParams.h>

#include <stheno/core/p2p/ft/AsyncStart.h>
#include <stheno/core/p2p/ft/AsyncStop.h>

#include <stheno/core/p2p/common/OverlayPeerInfo.h>


class UserService;

class ReplicationGroup;
typedef ACE_Strong_Bound_Ptr<ReplicationGroup, ACE_SYNCH_MUTEX> ReplicationGroupPtr;
class Replica;


class Overlay;

class FaultTolerance : public P2PService {
public:
    friend class Replica;
    friend class ReplicationGroup;
    FaultTolerance(Overlay* overlay);
    virtual ~FaultTolerance();

    //public API
    /// Entry point for FT, from core
    virtual void createReplicationGroup(
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            UUIDPtr& rgid,
            QoSManagerInterface* qosManager) throw (ServiceException&) = 0;

    //entry point for FT control svc
    virtual void createReplicationGroup(
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            UUIDPtr& rgid,            
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& peers,
            QoSManagerInterface* qosManager
            ) throw (ServiceException&) = 0;

    virtual const char* id();

    virtual void onReplicationGroupClose(UUIDPtr& rgid) = 0;

    // This could or not, use the discovery to find the resources
    virtual list<OverlayPeerInfoPtr>*
    findResources(UUIDPtr& sid, ServiceParamsPtr& params, UInt count) = 0;           
    
    virtual bool getReplicationGroup(UUIDPtr& rgid, ReplicationGroupPtr& rgPtr) = 0;     

    /*virtual QoSManagerInterface* getQoSManagerForReplicationGroup(
            UUIDPtr& sid,
            UUIDPtr& rgid,
            ServiceParamsPtr& params
            ) throw (ServiceException&) = 0;*/
};

typedef ACE_Strong_Bound_Ptr<FaultTolerance, ACE_Recursive_Thread_Mutex> FaultTolerancePtr;
#endif	/* FAULTTOLERANCE_H */
