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
 * File:   P3LeafMesh.h
 * Author: rmartins
 *
 * Created on September 15, 2010, 11:44 AM
 */

#ifndef P3LEAFMESH_H
#define	P3LEAFMESH_H
#include <ace/Synch.h>

#include <stheno/core/p2p/mesh/Mesh.h>
#include <euryale/net/CloseListener.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>

class LeafClientHandler;
typedef ACE_Strong_Bound_Ptr<LeafClientHandler, ACE_Recursive_Thread_Mutex> LeafClientHandlerPtr;
class P3LeafMeshSap;
class LeafPeer;

class P3LeafMesh : public Mesh, public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    //static const int DEFAULT_BIND_TIME = 2500; //us (2.5ms)
    //static const int DEFAULT_BIND_TIME = 1250; //us (1.25ms)
    static const int DEFAULT_BIND_TIME = 125; //us (1.25ms)
    
    P3LeafMesh(Overlay* overlay);
    virtual ~P3LeafMesh();

    virtual void getUUID(UUIDPtr& uuid);
    virtual void getFID(UUIDPtr& fid) throw (ServiceException&);

    //opens a channel and tries to allocate a remote service
    virtual void createRemoteService(
            const SAPInfo* hint,
            const UUIDPtr& uuid,
            const UUIDPtr& sid,
            ServiceParamsPtr& params,
            UUIDPtr& iid) throw (ServiceException&);

    //close local & remote
    virtual void closeRemoteService(UUIDPtr& uuid, UUIDPtr& sid) throw (ServiceException&);

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);

    virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&);

    virtual const char* getName();

    virtual PeerMapPtr& getPeerMap();
    //get local SAP
    void getSAP(SAPInfoPtr& sapInfo);

    void getCoordinatorDiscoverySAP(SAPInfoPtr& sapInfo) throw (ServiceException&);;
    void getCoordinatorMeshSAP(SAPInfoPtr& coordSAP) throw (ServiceException&);;
    void getCoordinatorUUID(UUIDPtr& uuid) throw (ServiceException&);;


    bool updateInfo(InfoUpdatePtr& updateInfoPtr);

    LeafPeer* getOverlay_i();

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;

    UUIDPtr m_fid;
    P3LeafMeshSap* m_sap;
    PeerMapPtr m_peerMap;
    //LeafClientHandler* m_client;
    LeafClientHandlerPtr m_client;

    UUIDPtr m_coordinatorUUID;
    SAPInfoPtr m_coordDiscoverySAP;
    SAPInfoPtr m_coordMeshSAP;

    void getClientHandler(LeafClientHandlerPtr& clientHandler);
    
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    virtual void close_i() throw (ServiceException&);
    void bind(bool firstTime=false) throw (ServiceException&);

    virtual void allocateQoS(QoSResources* qos) throw (ServiceException&);
    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    virtual void onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
protected:
    ACE_Time_Value m_start;
};

#endif	/* P3LEAFMESH_H */

