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
 * File:   P3Mesh.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 24, 2010, 4:00 PM
 */

#ifndef P3MESH_H
#define	P3MESH_H

#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellListener.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/mesh/net/P3MeshSAP.h>

#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>

class Cell;
class P3;
class SuperPeer;
class P3MeshSvcHandler;
class P3MeshCreateServicePacket;
class P3MeshRemoveServicePacket;

class P3Mesh : public Mesh, public CellListener {
public:
    P3Mesh(P3* overlay);
    virtual ~P3Mesh();

    virtual void getUUID(UUIDPtr& uuid);
    virtual void getFID(UUIDPtr& fid) throw (ServiceException&);

    virtual void createRemoteService(
            const SAPInfo* hint,
            const UUIDPtr& uuid,
            const UUIDPtr& sid,
            ServiceParamsPtr& params,
            UUIDPtr& iid) throw (ServiceException&);
    virtual void closeRemoteService(
            UUIDPtr& uuid,
            UUIDPtr& sid) throw (ServiceException&);

    virtual void onCellShutdown() {
    }

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);

    //virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&);

    virtual const char* getName();

    virtual PeerMapPtr& getPeerMap();

    virtual void getSAP(SAPInfoPtr& sapInfo);

    virtual void updateState(InfoUpdate* update) throw (ServiceException&);

    virtual void getNeighbors(list<PeerLocation>& l);

    int handleServiceCreate(P3MeshSvcHandler* channel, P3MeshCreateServicePacket* packet);
    int handleServiceRemove(P3MeshSvcHandler* channel, P3MeshRemoveServicePacket* packet);

    //Implementation specific
    SuperPeer* getOverlay_i();

    void getSensorList(list<P3PeerInfoPtr>& l);
    //PoL - Place of Launch
    void getPoL(const UUIDPtr& sid, P3PeerInfoPtr& l, list<UUIDPtr>& filter);

    int getPeerInfoByIID(UUIDPtr& sid, UUIDPtr& iid, SAPInfoPtr& pi);

    void getParentUUID(UUIDPtr& parentUUID) throw (ServiceException&);

    virtual void getDiscoverySAPInfo(UUIDPtr& uuid, SAPInfoPtr& si);

    bool addPeer(P3PeerInfoPtr& peer);

    virtual void onServiceCreation(ServiceInstanceInfoPtr& sii);
    virtual void onServiceUpdateIID(UUIDPtr& sid, UUIDPtr& oldIID, UUIDPtr& newIid);

    int getPeer(UUIDPtr& uuid, P3PeerInfoPtr& info);
    int getPeerByLevel(int treeLevel, P3PeerInfoPtr& info);
    int getRandomPeer(P3PeerInfoPtr& info);
    
    void getAudioSensors(list<SensorInfoPtr>& audioL);
    void getVideoSensors(list<SensorInfoPtr>& videoL);
    void getEventSensors(list<SensorInfoPtr>& eventL);
    void getActuatorSensors(list<SensorInfoPtr>& eventL);
    

    bool isRootFID();

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;

    PeerMapPtr m_peerMap;
    Cell* m_cell;
    P3MeshSAP* m_sap;

    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    virtual void close_i() throw (ServiceException&);
    void bind() throw (ServiceException&);

    virtual void allocateQoS(QoSResources* qos) throw (ServiceException&) {
    }
};

#endif	/* P3MESH_H */

