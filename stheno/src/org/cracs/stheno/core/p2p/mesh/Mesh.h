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
 * File:   Membership.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 12, 2010, 11:29 AM
 */

#ifndef MESH_H
#define	MESH_H
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/P2PService.h>
#include <stheno/core/p2p/mesh/exception/MeshException.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <stheno/service/UserService.h>
#include <stheno/core/p2p/common/PeerMap.h>

#include <stheno/core/p2p/mesh/MeshListener.h>

/*
 Mesh service has all the important information regarding
 * the node, e.g. services, etc (in p3: cells,...))
 */
class Overlay;
class Mesh : public P2PService {
public:
    friend class Overlay;

    virtual ~Mesh();

    //aka leaveMesh
    //close the MeshService, shutdowns:
    //a) user services
    //b) itself

    virtual void close() { //leaveMembership
        P2PService::close();
    }

    //QoS interface must be here
    //Easy access to runtime->getUUID()
    virtual void getUUID(UUIDPtr& uuid);
    //Get federation ID
    virtual void getFID(UUIDPtr& fid) throw (ServiceException&) = 0;

    //runtime public API, acts as a broker for accessing stheno core.   
    virtual void onServiceCreation(ServiceInstanceInfoPtr& sii) {}
    virtual void onServiceUpdateIID(UUIDPtr& sid,UUIDPtr& oldIID,UUIDPtr& newIid) {}
    
    virtual ServiceInfoPtr& getServiceInfo(UUIDPtr& serviceUUID);
    
    virtual int getPeerInfoByIID(UUIDPtr& sid,UUIDPtr& iid,PeerInfoPtr& pi);

    //serviceParams includes the information about FT, with FTParams    
    virtual void createLocalService(UUIDPtr& sid, ServiceParamsPtr& params,UUIDPtr& iid) throw (ServiceException&);

    //opens a channel and tries to allocate a remote service, using mesh sid@uuid
    //remote peer uses createLocalService(...) that in turn negociates with stheno core.
    virtual void createRemoteService(const SAPInfo* hint,const UUIDPtr& uuid, const UUIDPtr& sid, ServiceParamsPtr& params,UUIDPtr& iid) throw (ServiceException&) = 0;

    //close local & remote
    //virtual void stopService(UUIDPtr& iid) = 0;    
    virtual void closeLocalService(UUIDPtr& sid) throw (ServiceException&);
    virtual void closeRemoteService(UUIDPtr& uuid, UUIDPtr& sid) throw (ServiceException&) = 0;

    virtual const char* id();

    virtual PeerMapPtr& getPeerMap() = 0;

protected:
    //PeerMap m_peerMap;
    Mesh(Overlay* overlay);

    virtual void allocateQoS(QoSResources* qos) throw (ServiceException&) = 0;
    //Access local repository(for built-in or plugin service) and spawns an instances
    UserService* createServiceInstance(UUIDPtr& sid) throw (ServiceException&);

    //Mesh interface/implementation of service-specific discovery primitives
    //Check is the federation fails, i.e., send a ping in a cell
    virtual void addFederationListener(MeshListener* l,UUIDPtr& fid) throw(ServiceException&);
    //the coordinator for a federation informs of a membership change
    virtual void addPeerListener(MeshListener* l,UUIDPtr& fid, UUIDPtr& peerID) throw(ServiceException&);
};

//typedef ACE_Strong_Bound_Ptr<Membership, ACE_Recursive_Thread_Mutex> MembershipPtr;
typedef ACE_Strong_Bound_Ptr<Mesh, ACE_Recursive_Thread_Mutex> MeshPtr;

#endif	/* MESH_H */

