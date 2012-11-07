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
 * File:   P3ReplicationGroup.h
 * Author: rmartins
 *
 * Created on November 26, 2010, 9:54 AM
 */

#ifndef P3REPLICATIONGROUP_H
#define	P3REPLICATIONGROUP_H

#include <stheno/core/p2p/ft/ReplicationGroup.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataSAP.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataChannelManager.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlChannelManager.h>


class FTControlSvcHandler;
class FTDataSvcHandler;
class FTAddReplicaPacket;
class FTRemoveReplicaPacket;

class P3ReplicationGroup : public ReplicationGroup {
public:
    P3ReplicationGroup(
            FaultTolerance* ft,
            UUIDPtr& rgid,
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            QoSManagerInterface* qosManager) throw (ServiceException&);
    //Replica bootstrap
    P3ReplicationGroup(
            FaultTolerance* ft,
            UUIDPtr& rgid,
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            OverlayPeerInfoPtr& primaryPeer,
            list<OverlayPeerInfoPtr>& replicas,
            QoSManagerInterface* qosManager) throw (ServiceException&);
    virtual ~P3ReplicationGroup();

    virtual bool updateFTDataSAP(UUIDPtr& uuid, SAPInfoPtr& ftDataSAP);
    virtual bool removeReplica(UUIDPtr& uuid);

    virtual void open() throw (ServiceException&);

    virtual bool hasFTDataSAP();
    virtual SAPInfoPtr& getFTDataSAPInfo() throw (ServiceException&);

    virtual FTControlChannelManager& getControlChannelManager();

    virtual void onPeerClosure(UUIDPtr& uuid);

    bool isThisPeerPrimary();

    bool getPrimaryUUID(UUIDPtr& uuid);
    bool isPeerPrimary(UUIDPtr& uuid);
    bool isPeerReplica(UUIDPtr& uuid);


    int handleAddReplicaPacket(FTControlSvcHandler* svcHandler, FTAddReplicaPacket* packet);
    int handleRemoveReplicaPacket(FTControlSvcHandler* svcHandler, FTRemoveReplicaPacket* packet);
    
    int handleReplicationPacket(FTDataSvcHandler* svcDataHandler,FTDataReplicationPacket* replicationPacket);
    void replicate(ACE_Message_Block* payload) throw (ServiceException&);

protected:    
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    FTDataSAP m_dataSAP;
    FTDataChannelManager m_clientDataManager;
    FTControlChannelManager m_clientControlManager;
    bool m_debugP3ReplicationGroup;

    void openPrimary() throw (ServiceException&);
    void openReplica() throw (ServiceException&);

    virtual bool addReplicaAsPrimary(OverlayPeerInfoPtr& replica);
    virtual bool addReplicaAsReplica(OverlayPeerInfoPtr& replica);

    virtual bool bindControlToReplica(OverlayPeerInfoPtr& replica);
    virtual bool bindDataToReplica(OverlayPeerInfoPtr& replica);

    virtual bool prebindControlToReplica(OverlayPeerInfoPtr& replica);
    virtual bool prebindDataToReplica(OverlayPeerInfoPtr& replica);

    virtual bool fireOnChangeToPrimary();

private:
    void printMembers();
};
typedef ACE_Strong_Bound_Ptr<P3ReplicationGroup, ACE_SYNCH_MUTEX> P3ReplicationGroupPtr;

#endif	/* P3REPLICATIONGROUP_H */

