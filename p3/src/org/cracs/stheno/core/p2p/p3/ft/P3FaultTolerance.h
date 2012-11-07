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
 * File:   P3FaultTolerance.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 24, 2010, 4:06 PM
 */

#ifndef P3FAULTTOLERANCE_H
#define	P3FAULTTOLERANCE_H

#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlSAP.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroupManager.h>

class P3FaultTolerance : public FaultTolerance {
public:
    P3FaultTolerance(Overlay* overlay);
    virtual ~P3FaultTolerance();

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);
    virtual ServiceAbstract* duplicate() throw (ServiceException&);
    virtual const char* getName();
    virtual void getSAP(SAPInfoPtr& sapInfo);
    virtual const char* id();

    virtual void open_i(
            ServiceParamsPtr& params,
            int fttype) throw (ServiceException&);

    virtual list<OverlayPeerInfoPtr>* findResources(
            UUIDPtr& sid,
            ServiceParamsPtr& params,
            UInt count);


    void createReplicationGroup(
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            UUIDPtr& rgid,
            QoSManagerInterface* qosManager
            ) throw (ServiceException&);
    void createReplicationGroup(
            ServiceAbstractPtr& svcPtr,
            ServiceParamsPtr& params,
            UUIDPtr& rgid,            
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& replicas,
            QoSManagerInterface* qosManager
            ) throw (ServiceException&);

    virtual void onReplicationGroupClose(UUIDPtr& rgid);
    //bool getReplicationGroup(UUIDPtr& rgid, P3ReplicationGroupPtr& rgPtr);        
    bool getReplicationGroup(UUIDPtr& rgid, ReplicationGroupPtr& rgPtr);

protected:
    FTControlSAP m_controlSAP;
    P3ReplicationGroupManager m_replicationGroupManager;

    void getFilterPeers(list<OverlayPeerInfoPtr>* replicas, list<UUIDPtr>& filterPeers);
};
#endif	/* P3FAULTTOLERANCE_H */

