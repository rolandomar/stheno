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
 * File:   P3LeafMesh.cpp
 * Author: rmartins
 * 
 * Created on September 15, 2010, 11:44 AM
 */

#include "P3LeafMesh.h"
#include <euryale/common/sleep/IncrementalSleep.h>
//#include <stheno/core/p2p/p3/superpeer/cell/discovery/CellDiscovery.h>
#include <stheno/core/p2p/p3/leaf/mesh/LeafMeshDiscovery.h>
#include <stheno/core/p2p/p3/leaf/mesh/net/P3LeafMeshSap.h>
#include <stheno/core/p2p/p3/leaf/mesh/net/LeafClientHandler.h>
#include <ace/Sock_Connect.h>
#include <ace/Connector.h>
#include <stheno/core/p2p/p3/leaf/LeafPeer.h>

#include <stheno/core/p2p/p3/mesh/net/P3MeshClientHandler.h>

#include <stheno/common/TraceRuntime.h>

P3LeafMesh::P3LeafMesh(Overlay* overlay) : Mesh(overlay), m_client(0) {
}

P3LeafMesh::~P3LeafMesh() {
}

void P3LeafMesh::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::open_i(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (isStarting() /*|| isResuming()*/) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::open_i starting(%d)\n"), m_status));
        //Task::activate();
        int maxBindTries = 10000;
        //IncrementalSleep sleeper(1,0);
        //IncrementalSleep sleeper(0,1000);
        IncrementalSleep sleeper(0, DEFAULT_BIND_TIME);
        m_start = ACE_OS::gettimeofday();
        for (int i = 0; i < maxBindTries; i++) {
            try {
                bind(true);
                return;
            } catch (ServiceException& bindEx) {
                //discard bindEx
                sleeper.sleep();
            }
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::open(): bind failed\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::open(): error\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
}

void P3LeafMesh::close_i() throw (ServiceException&) {
}

void P3LeafMesh::bind(bool firstTime) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::bind():open\n")));
    UUIDPtr runtimeUUID;
    this->getUUID(runtimeUUID);    
    LeafMeshDiscovery* discovery = new LeafMeshDiscovery(runtimeUUID);    
    try {
        discovery->open();
    } catch (CellException& ex) {
        delete discovery;
        throw ServiceException("Discovery failed to open!");
    }        
    CoordinatorInfo* coordInfo = discovery->requestCoordinator();
    if (coordInfo == 0) {
        delete discovery;
        throw ServiceException("Error connecting to coordinator!");
    }    
  
    m_coordMeshSAP = coordInfo->getMeshSap();
    m_fid = coordInfo->getCellID();
    m_coordDiscoverySAP = coordInfo->getDiscoverySap();
    m_coordinatorUUID = coordInfo->getPID();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) INFO: P3LeafMesh::bind() - DiscoverySAP=%@\n"), m_coordDiscoverySAP.get()));
    ACE_Connector<LeafClientHandler, ACE_SOCK_Connector> connector;
    CellID* cellID = new CellID(*coordInfo->getCellID().get());
    CellIDPtr cellIDPtr(cellID);

    //LeafClientHandler* client = new LeafClientHandler(coordInfo->getPID(), cellIDPtr, this, false, false, 0, 0, 0, 0);
    ThreadPerConnection *tpc = new ThreadPerConnection();
    /*ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt*/ 
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);
    LeafClientHandler* client = new LeafClientHandler(coordInfo->getPID(), cellIDPtr, this, false, false, tpcPrt, 0, 0, 0);
    
    
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(client);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(client);
    connector.reactor(tpc->getResources()->getReactor());

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh - Connecting...(%s) client=%@!\n"),
            coordInfo->getPID()->toString().c_str(), client));
    if (connector.connect(client, coordInfo->getCellCoordinatorEndpoint()->getAddr()) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3LeafMesh - Error Connecting...(%s) client=%@!\n"),
                coordInfo->getPID()->toString().c_str(), client));
        perror("P3LeafMesh=");
        /*ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)connect failed:")));*/
        delete coordInfo;

        delete client;
        delete discovery;

        throw ServiceException("Error connecting to coordinator!");
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:bind() - Connect OK!\n")));
        delete discovery;
        delete coordInfo;
        m_client.reset(client);
        if (m_client->asynchronous(true,false) == -1) {
            //delete m_client;
            //m_client = 0;
            m_client.reset(0);
            throw ServiceException("Error connecting to coordinator (2)!");
        }        
        bool joinRet = client->joinMesh(getOverlay_i()->getType());
        //m_client = client;        
        if (m_client->setCloseListener(this) == -1) {
            //delete m_client;
            //m_client = 0;
            m_client.reset(0);
            throw ServiceException("Error connecting to coordinator (2)!");
        }        
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:bind() - join(%d)\n"), joinRet));
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:bind() - join end!\n")));
    UUIDPtr fid;
    this->getFID(fid);
    TraceRuntimeSingleton::instance()->logBindToMesh(runtimeUUID,fid, m_coordinatorUUID, m_fid);
    if(firstTime){
        ACE_Time_Value end = ACE_OS::gettimeofday();
        end -= m_start;
        TraceRuntimeSingleton::instance()->logMembershipTime(end);
    }else{
        ACE_Time_Value end = ACE_OS::gettimeofday();
        end -= m_start;
        TraceRuntimeSingleton::instance()->logMembershipRebindTime(end);
    }
    //CellDiscovery* discovery = new CellDiscovery(this->getUUID());
    //discovery->open();
    //CellReply* info = discovery->requestCell(CellID::INVALID_CELL_ID_UUIDPTR);

}

void P3LeafMesh::getSAP(SAPInfoPtr& s) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    s = m_sap->getSAPInfo();
}

void P3LeafMesh::getCoordinatorDiscoverySAP(SAPInfoPtr& sapInfo) throw (ServiceException&){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    sapInfo = m_coordDiscoverySAP;
    //return true;
}

void P3LeafMesh::getCoordinatorMeshSAP(SAPInfoPtr& coordSAP) throw (ServiceException&){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    coordSAP = m_coordMeshSAP;
}

void P3LeafMesh::getCoordinatorUUID(UUIDPtr& uuid) throw (ServiceException&){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    uuid =  m_coordinatorUUID;
}

void P3LeafMesh::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:onClose() - (%@)\n"), channel));
    /*if(m_client != channel){
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:onClose() - NOT m_client????????????(%@)\n"),channel));    
    }*/
    channel->setCloseListener(0);
    m_client.reset(0);
    //delete channel;
    //m_client = 0;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:onClose() - after delete(%@)\n"), channel));
    //m_client = 0;
    IncrementalSleep sleeper(0, DEFAULT_BIND_TIME);
    m_start = ACE_OS::gettimeofday();
    while (true) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh:onClose() - loop with delete channel of (%@)\n"), channel));
        try {
            bind();
            break;
        } catch (ServiceException& ex) {
            //retry
            sleeper.sleep();
        }
    }
}

LeafPeer* P3LeafMesh::getOverlay_i() {
    return static_cast<LeafPeer*> (m_overlay);
}

//opens a channel and tries to allocate a remote service

void P3LeafMesh::createRemoteService(const SAPInfo* hint, const UUIDPtr& uuid, const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (hint == 0) {
        throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
    }
    Endpoint endpoint;
    ACE_Connector<P3MeshClientHandler, ACE_SOCK_Connector> connector;
    hint->getFirstEndpoint(endpoint);
    QoSEndpoint qosE = *(endpoint.getQoS());
    UUIDPtr runtimeUUID;
    getUUID(runtimeUUID);
    UUIDPtr fid;
    getFID(fid);
    P3MeshClientHandler* clientHandler = new P3MeshClientHandler(
            runtimeUUID,
            fid,
            qosE,
            false, false, 0, 0, 0, 0);

    if (connector.connect(clientHandler, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: P3Mesh::createRemoteService - connect failed:")));
        clientHandler->close();
        clientHandler = 0;
        delete clientHandler;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh::createRemoteService - Connect OK!\n")));
    }

    int ret = clientHandler->createService(params, iid);
    clientHandler->close();
    delete clientHandler;
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh::createRemoteService - failed to create, not enough resources\n")));
        throw ServiceException(ServiceException::INSUFFICIENT_RESOURCES);
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3LeafMesh::createRemoteService - service created!\n")));
}

//close local & remote

void P3LeafMesh::closeRemoteService(UUIDPtr& uuid, UUIDPtr& sid) throw (ServiceException&) {
}

QoSResources* P3LeafMesh::calculateQoSResources(ServiceParamsPtr& params) {
    return 0;
}

list<EndpointPtr>& P3LeafMesh::getEndpoints() throw (ServiceException&) {
    throw ServiceException(ServiceException::INVALID_ARGUMENT);
}

bool P3LeafMesh::updateInfo(InfoUpdatePtr& updateInfoPtr) {
    /*ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, false);
    if (m_client.null()) {
        return false;
    }*/
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,false);
    LeafClientHandlerPtr clientHandler;
    getClientHandler(clientHandler);
    if (clientHandler.null()) {
        return false;
    }
    ace_mon.release();
    return clientHandler->updateInfo(updateInfoPtr);
}

void P3LeafMesh::getUUID(UUIDPtr& uuid) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    Mesh::getUUID(uuid);
}

void P3LeafMesh::getFID(UUIDPtr& fid) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    fid = m_fid;
}

const char* P3LeafMesh::getName() {
    return "P3LeafMesh";
}

PeerMapPtr& P3LeafMesh::getPeerMap() {
    return m_peerMap;
}

void P3LeafMesh::allocateQoS(QoSResources* qos) throw (ServiceException&) {
}

void P3LeafMesh::onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
}

void P3LeafMesh::getClientHandler(LeafClientHandlerPtr& clientHandler) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    clientHandler = m_client;
}