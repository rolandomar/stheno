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
 * File:   ReplicaGroup.h
 * Author: rmartins
 *
 * Created on August 4, 2010, 3:45 PM
 */

#ifndef REPLICATIONGROUP_H
#define	REPLICATIONGROUP_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/Types.h>
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/core/p2p/ft/Replica.h>

//Forward references
class QoSManagerInterface;
class ReplicationGroup {
public:
    //Primary bootstrap
    ReplicationGroup(
            FaultTolerance* ft,   
            UUIDPtr& rgid,
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            QoSManagerInterface* qosManager) throw (ServiceException&);
    //Replica bootstrap
    ReplicationGroup(
            FaultTolerance* ft, 
            UUIDPtr& rgid,            
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            OverlayPeerInfoPtr& primaryPeer,
            list<OverlayPeerInfoPtr>& replicas,
            QoSManagerInterface* qosManager) throw (ServiceException&);

    virtual ~ReplicationGroup();
    
    /**
     * Add a new replica.
     * @param replica
     */
    virtual bool addReplica(OverlayPeerInfoPtr& replica);
    /**
     * replication group n-n connections
     * for data replication (to transfer user specific state/operations)
     * @param uuid
     * @param ftDataSAP
     * @return 
     */
    virtual bool updateFTDataSAP(UUIDPtr& uuid,SAPInfoPtr& ftDataSAP) = 0;
    virtual bool removeReplica(UUIDPtr& uuid) = 0;

    //UUID for this replica group
    virtual UUIDPtr& getGroupUUID();

    virtual void open() throw (ServiceException&) = 0;
    virtual void close();

    virtual bool isPrimary();    
    
    virtual bool hasFTDataSAP() = 0;
    virtual SAPInfoPtr& getFTDataSAPInfo() throw (ServiceException&) = 0;
    
    virtual ServiceAbstractPtr& getService();
    OverlayPeerInfoPtr& getPrimary();    
    list<OverlayPeerInfoPtr>& getReplicaList();    
    void getSID(UUIDPtr& sid);    
    ServiceParamsPtr& getServiceParams();    
    FaultTolerance* getFaultToleranceService();
    
    virtual void replicate(ACE_Message_Block* payload) throw (ServiceException&) = 0;
    
    QoSManagerInterface* getQoSManager();
    
protected:
    UUIDPtr m_groupUUID;
    //can be null
    ServiceAbstractPtr m_svcPtr;
    //not null
    UUIDPtr m_sid;        
    OverlayPeerInfoPtr m_primaryPeer;
    list<OverlayPeerInfoPtr> m_replicas;
    //list<OverlayPeerInfoPtr> m_bindedPeers;
    ServiceParamsPtr m_params;
    bool m_primary;
    FaultTolerance* m_ft;
    bool m_closed;
    //QoS
    QoSManagerInterface* m_qosManager;
    
    
    
    virtual bool addReplicaAsPrimary(OverlayPeerInfoPtr& replica) = 0;
    virtual bool addReplicaAsReplica(OverlayPeerInfoPtr& replica) = 0;

    virtual bool isThisPeer(UUIDPtr& uuid);
    /**
     * 
     * @param uuid
     * @return Returns -1 if replica doesn't belong to the group, 0 if precedent
     * , 1 otherwise
     */
    virtual int isReplicaPrecedent(UUIDPtr& uuid);    
};

typedef ACE_Strong_Bound_Ptr<ReplicationGroup, ACE_SYNCH_MUTEX> ReplicationGroupPtr;


#endif	/* REPLICATIONGROUP_H */

