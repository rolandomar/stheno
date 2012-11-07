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
 * File:   P3ReplicationGroup.cpp
 * Author: rmartins
 * 
 * Created on November 26, 2010, 9:54 AM
 */

#include "P3ReplicationGroup.h"
#include <stheno/core/SthenoCoreInterface.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlClientHandler.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlSvcHandler.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataClientHandler.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataSvcHandler.h>

#include <stheno/core/QoSManager.h>

P3ReplicationGroup::P3ReplicationGroup(
        FaultTolerance* ft,
        UUIDPtr& rgid,
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        QoSManagerInterface* qosManager) throw (ServiceException&) :
ReplicationGroup(ft, rgid, svcPtr, params, qosManager),
m_dataSAP(this),
m_clientDataManager(this),
m_clientControlManager(this) {
    m_debugP3ReplicationGroup = RuntimeConfiguration::isClassDebugged("P3ReplicationGroup");
}

P3ReplicationGroup::P3ReplicationGroup(
        FaultTolerance* ft,
        UUIDPtr& rgid,
        ServiceAbstractPtr& svcPtr,
        ServiceParamsPtr& params,
        OverlayPeerInfoPtr& primaryPeer,
        list<OverlayPeerInfoPtr>& replicas,
        QoSManagerInterface* qosManager) throw (ServiceException&) :
ReplicationGroup(ft, rgid, svcPtr, params, primaryPeer, replicas, qosManager),
m_dataSAP(this),
m_clientDataManager(this),
m_clientControlManager(this) {
    m_debugP3ReplicationGroup = RuntimeConfiguration::isClassDebugged("P3ReplicationGroup");
}

P3ReplicationGroup::~P3ReplicationGroup() {
}

void P3ReplicationGroup::openPrimary() throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - PRIMARY\n")));
    MeshPtr meshPtr;
    try {
        m_ft->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw ServiceException(ServiceException::LOW_LVL_P2P_UNAVAILABLE);
    }

    SAPInfoPtr meshSapInfo;
    meshPtr->getSAP(meshSapInfo);
    SAPInfoPtr ftSapInfo;
    m_ft->getSAP(ftSapInfo);
    UUIDPtr uuid;
    m_ft->getOverlay()->getUUID(uuid);
    UUIDPtr fid;
    meshPtr->getFID(fid);
    OverlayPeerInfo* primary = new OverlayPeerInfo(
            uuid,
            fid,
            meshSapInfo,
            ftSapInfo,
            m_dataSAP.getSAPInfo()
            );
    //IID belongs to the replication group,
    //the primary must use this value
    //m_iid.reset(UUID::generateUUID());

    m_primaryPeer.reset(primary);
    //replica count
    UInt replicaCount = this->m_params->getFTParams()->getReplicaCount();
    //find the resources (in the mesh)
    list<OverlayPeerInfoPtr>* replicaPeers = m_ft->findResources(
            this->m_sid,
            this->m_params,
            replicaCount
            );
    if (replicaPeers == 0) {
        throw ServiceException(ServiceException::INSUFFICIENT_RESOURCES);
    }
    //Add replicas
    list<OverlayPeerInfoPtr>::iterator iter = replicaPeers->begin();
    while (iter != replicaPeers->end()) {
        addReplica(*iter);
        iter++;
    }
    //delete temp information
    delete replicaPeers;
    //debug: print members
    this->printMembers();
    //Done start service here
    //bootstrap service
    //use the replication group IID    
    //m_svcPtr->setIID(m_groupUUID);

    // this QoS manager was provided by the core
    //QoSManagerInterface* qosManager = this->getQoSManager();
    //m_svcPtr->open(m_params,ServiceAbstract::FT_PRIMARY,qosManager);
    //new, bind group
    ReplicationGroupPtr rgPtr;
    m_ft->getReplicationGroup(this->getGroupUUID(), rgPtr);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - PRIMARY middle\n")));
    m_svcPtr->setReplicationGroup(rgPtr);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - PRIMARY final OK\n")));
}

void P3ReplicationGroup::openReplica() throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - REPLICA() replicaCount=%d\n"), m_replicas.size()));
    /*list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    while (iter != m_replicas.end()) {
        addReplica(*iter);        
        iter++;
    }*/
    //add itself
    MeshPtr meshPtr;
    try {
        m_ft->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw ServiceException(ServiceException::LOW_LVL_P2P_UNAVAILABLE);
    }
    SAPInfoPtr meshSapInfo;
    meshPtr->getSAP(meshSapInfo);
    SAPInfoPtr ftSapInfo;
    m_ft->getSAP(ftSapInfo);
    UUIDPtr uuid;
    m_ft->getOverlay()->getUUID(uuid);
    UUIDPtr fid;
    meshPtr->getFID(fid);

    OverlayPeerInfo* ourReplicaInfo = new OverlayPeerInfo(
            uuid,
            fid,
            meshSapInfo,
            ftSapInfo,
            m_dataSAP.getSAPInfo()
            );
    OverlayPeerInfoPtr replicaInfoPtr(ourReplicaInfo);
    //addReplica(replicaInfoPtr);
    m_replicas.push_back(replicaInfoPtr);
    this->printMembers();
    //Bootstrap service!
    //m_ft->getOverlay()->getRuntime()->createLocalServiceReplica(m_sid,m_params,m_svcPtr);    
    //debug
    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - REPLICA() service lanuched (%@) iid=%s!\n"),
            m_svcPtr.get(),m_svcPtr->getIID()->toString().c_str()));*/
    //remove this (just for remembering)
    //m_svcPtr->open(m_params,ServiceAbstract::FT_REPLICA);
    ReplicationGroupPtr rgPtr;
    m_ft->getReplicationGroup(this->getGroupUUID(), rgPtr);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - REPLICA() middle replicaCount=%d\n"), m_replicas.size()));
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - REPLICA() middle replicaCount=%d\n"), m_replicas.size()));
    m_svcPtr->setReplicationGroup(rgPtr);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open() - REPLICA() final replicaCount=%d\n"), m_replicas.size()));

}

void P3ReplicationGroup::open() throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::open()()\n")));
    m_closed = false;
    list<QoSEndpoint> qosList;
    //QoSEndpoint hrt(QoS::HRT, CPUQoS::MED_RT_PRIO);
    QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    qosList.push_back(hrt);
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    m_dataSAP.open(itf, qosList);

    if (isPrimary()) {
        //open as primary
        openPrimary();
    } else {
        //open as replica
        openReplica();
    }
}

void P3ReplicationGroup::printMembers() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamService::printMembers():\n")));
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("\tPrimary=%s\n"), this->m_primaryPeer->getUUID()->toString().c_str()));
    list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    while (iter != m_replicas.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("\t\tReplica uuid=%s\n"), (*iter)->getUUID()->toString().c_str()));
        iter++;
    }
}

bool P3ReplicationGroup::bindControlToReplica(OverlayPeerInfoPtr& replica) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_Connector<FTControlClientHandler, ACE_SOCK_Connector> connector;
    Endpoint endpoint;
    replica->getFTSAPInfo()->getFirstEndpoint(endpoint);
    //printf("==============bindControlToReplica endpoint=%s\n", endpoint.toString().c_str());

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    FTControlClientHandler* ftControlClient = new FTControlClientHandler(
            this,
            replica->getUUID(),
            replica->getFID(),
            this->getGroupUUID(),
            false, false, tpcPrt, 0, 0, 0);

    printMembers();
    SAPInfoPtr ftDataSAPInfo;

    //*(getGroupMembers()));
    tpc->bind(ftControlClient);
    tpc->open();
    //tpc->bind(ftControlClient);
    connector.reactor(tpc->getResources()->getReactor());

    if (connector.connect(ftControlClient, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: P3ReplicationGroup::bindControlToReplica - connect failed:")));
        ftControlClient->close();
        delete ftControlClient;
        return false;
    } else {
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindControlToReplica - Connect to replica OK! UUID=%s\n"),
                    replica->getUUID()->toString().c_str()));
        }
    }
    UUIDPtr sid;
    getSID(sid);
    ftControlClient->initSession(
            sid,
            m_groupUUID,
            //m_iid,
            this->getServiceParams(),
            this->getPrimary(),
            this->getReplicaList(),
            ftDataSAPInfo);
    if (m_debugP3ReplicationGroup) {
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindControlToReplica() - update(dataft) = (%@)\n"),
                ftDataSAPInfo.get()
                ));
    }
    if (m_clientControlManager.add(ftControlClient) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindControlToReplica - to add to group! UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        return false;
    }
    //this->updateFTDataSAP(replica->getUUID(), ftDataSAPInfo); //why???
    replica->setFTDataSAP(ftDataSAPInfo);

    if (ftControlClient->asynchronous(true, false) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindControlToReplica - to aync! UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        ftControlClient->close();
        return false;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindControlToReplica -OK UUID=%s\n"),
            replica->getUUID()->toString().c_str()));
    return true;
}

bool P3ReplicationGroup::bindDataToReplica(OverlayPeerInfoPtr& replica) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_Connector<FTDataClientHandler, ACE_SOCK_Connector> connector;
    Endpoint endpoint;
    replica->getFTDataSAPInfo()->getFirstEndpoint(endpoint);
    //printf("==============bindDataToReplica endpoint=%s\n", endpoint.toString().c_str());
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    FTDataClientHandler* ftDataClient = new FTDataClientHandler(this,
            replica->getUUID(),
            replica->getFID(),
            this->getGroupUUID(),
            false, false, tpcPrt, 0, 0, 0);

    CPUReservation* reserve = 0;
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    if (this->m_ft->getQoSManager() != 0) {
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(ftDataClient);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(ftDataClient);
    connector.reactor(tpc->getResources()->getReactor());

    if (connector.connect(ftDataClient, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)ERROR: P3ReplicationGroup::bindDataToReplica - connect failed:")));
        ftDataClient->close();
        delete ftDataClient;
        return false;
    } else {

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindDataToReplica - Connect Data to Replica - OK! UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
    }

    ftDataClient->initSession();
    if (m_clientDataManager.add(ftDataClient) == -1) {
        return false;
    }

    if (ftDataClient->asynchronous(true, false) == -1) {
        ftDataClient->close();
        return false;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::bindDataToReplica - done UUID=%s\n"),
            replica->getUUID()->toString().c_str()));
    return true;
}

bool P3ReplicationGroup::prebindControlToReplica(OverlayPeerInfoPtr& replica) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_Connector<FTControlClientHandler, ACE_SOCK_Connector> connector;
    Endpoint endpoint;
    replica->getFTSAPInfo()->getFirstEndpoint(endpoint);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica - SAP=%s\n"), endpoint.toString().c_str()));
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    FTControlClientHandler* ftControlClient = new FTControlClientHandler(
            this,
            replica->getUUID(),
            replica->getFID(),
            this->getGroupUUID(),
            false, false, tpcPrt, 0, 0, 0);

    printMembers();
    SAPInfoPtr ftDataSAPInfo;

    //*(getGroupMembers()));
    CPUReservation* reserve = 0;
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    if (this->m_ft->getQoSManager() != 0) {
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(ftControlClient);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(ftControlClient);
    connector.reactor(tpc->getResources()->getReactor());

    if (connector.connect(ftControlClient, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: P3ReplicationGroup::prebindControlToReplica - connect failed:")));
        ftControlClient->close();
        delete ftControlClient;
        return false;
    } else {
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica - Connect to replica OK! UUID=%s\n"),
                    replica->getUUID()->toString().c_str()));
        }
    }
    ftControlClient->preBindSession();
    if (m_debugP3ReplicationGroup) {
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica() - update(dataft) = (%@)\n"),
                ftDataSAPInfo.get()
                ));
    }
    if (m_clientControlManager.add(ftControlClient) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica - failed to add to group! UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        return false;
    }

    if (ftControlClient->asynchronous(true, false) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica - failed to async! UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        ftControlClient->close();
        return false;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindControlToReplica - OK! UUID=%s\n"),
            replica->getUUID()->toString().c_str()));
    return true;
}

bool P3ReplicationGroup::prebindDataToReplica(OverlayPeerInfoPtr& replica) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_Connector<FTDataClientHandler, ACE_SOCK_Connector> connector;
    Endpoint endpoint;
    replica->getFTDataSAPInfo()->getFirstEndpoint(endpoint);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindDataToReplica - SAP=%s\n"), endpoint.toString().c_str()));
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    FTDataClientHandler* ftDataClient = new FTDataClientHandler(this,
            replica->getUUID(),
            replica->getFID(),
            this->getGroupUUID(),
            false, false, tpcPrt, 0, 0, 0);

    CPUReservation* reserve = 0;
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    if (this->m_ft->getQoSManager() != 0) {
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(ftDataClient);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(ftDataClient);
    connector.reactor(tpc->getResources()->getReactor());

    if (connector.connect(ftDataClient, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: P3ReplicationGroup::prebindDataToReplica - connect failed:")));
        ftDataClient->close();
        delete ftDataClient;
        return false;
    } else {
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindDataToReplica - Connect Data to Replica - OK! UUID=%s\n"),
                    replica->getUUID()->toString().c_str()));
        }
    }

    ftDataClient->preBindSession();

    if (m_clientDataManager.add(ftDataClient) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindDataToReplica - failed to add to group UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        return false;
    }

    if (ftDataClient->asynchronous(true, false) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindDataToReplica - failed to add to async UUID=%s\n"),
                replica->getUUID()->toString().c_str()));
        ftDataClient->close();
        return false;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::prebindDataToReplica - OK! UUID=%s\n"),
            replica->getUUID()->toString().c_str()));
    return true;
}

bool P3ReplicationGroup::addReplicaAsPrimary(OverlayPeerInfoPtr& replica) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsPrimary() (uuid,fid,ft,dataft) = (%s,%s,%@,%@)\n"),
            replica->getUUID()->toString().c_str(),
            replica->getFID()->toString().c_str(),
            replica->getFTSAPInfo().get(),
            replica->getFTDataSAPInfo().get()
            ));
    bool bindRet = false;
    //bind to control
    bindRet = bindControlToReplica(replica);
    if (!bindRet) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3ReplicationGroup::addReplicaAsPrimary()- fail to bind control\n")));
        return false;
    }
    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsPrimary() - update(uuid,fid,ft,dataft) = (%s,%s,%@,%@)\n"),
            replica->getUUID()->toString().c_str(),
            replica->getFID()->toString().c_str(),
            replica->getFTSAPInfo().get(),
            replica->getFTDataSAPInfo().get()
            ));

    //provide data channel
    bindRet = bindDataToReplica(replica);
    if (!bindRet) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3ReplicationGroup::addReplicaAsPrimary()- fail to data control\n")));
        return false;
    }
    //update the group except the newly replica
    if (m_debugP3ReplicationGroup) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsPrimary() - before update group REPLICA UUID=%s\n"), replica->getUUID()->toString().c_str()));
    }
    int groupUpdateRet = m_clientControlManager.addReplica(replica->getUUID(), replica->getFID(), replica);
    if (m_debugP3ReplicationGroup) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsPrimary() - after update group\n")));
    }
    if (groupUpdateRet == 0) {
        m_replicas.push_back(replica);
    }
    return (groupUpdateRet == 0);
}

bool P3ReplicationGroup::addReplicaAsReplica(OverlayPeerInfoPtr& replica) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica() before lock\n")));
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica() after lock\n")));
    int ret = 1; //isReplicaPrecedent(replica->getUUID());
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - -1!!!!!!!\n")));
        return true;
    }
    //replica is on the right
    if (ret == 1) {
        //printf("INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the rigth, so binding...\n");
        bool bindRet = false;
        //bind to control
        //printf("INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the rigth, so binding control...\n");
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the rigth, so binding control...\n")));
        }
        bindRet = prebindControlToReplica(replica);
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - control ret=%d\n"), bindRet));
        }
        //provide data channel
        //printf("INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the rigth, so binding data...\n");
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the rigth, so binding data...\n")));
        }
        bindRet = prebindDataToReplica(replica);
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - data ret=%d\n"), bindRet));
        }
        if (bindRet) {
            m_replicas.push_back(replica);
        }
        return bindRet;
    } else {
        if (m_debugP3ReplicationGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::addReplicaAsReplica - replica is on the left, so not binding\n")));
        }
        m_replicas.push_back(replica);
        return true;
    }
}

bool P3ReplicationGroup::updateFTDataSAP(UUIDPtr& uuid, SAPInfoPtr& ftDataSAP) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    while (iter != m_replicas.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            (*iter)->setFTDataSAP(ftDataSAP);
            break;
        }
        iter++;
    }
    return false;
}

bool P3ReplicationGroup::removeReplica(UUIDPtr& uuid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    if (isThisPeer(uuid)) {
        //Removing ourselfs
        //clear the group and notify FT
        close();
        m_ft->onReplicationGroupClose(getGroupUUID());
        return true;
    }

    m_clientControlManager.remove(uuid);
    m_clientDataManager.remove(uuid);

    bool flag = false;
    list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    while (iter != m_replicas.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            m_replicas.erase(iter);
            flag = true;

            break;
        }
        iter++;
    }

    return flag;
}

bool P3ReplicationGroup::hasFTDataSAP() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    return !m_dataSAP.getSAPInfo().null();
}

SAPInfoPtr& P3ReplicationGroup::getFTDataSAPInfo() throw (ServiceException&) {
    return m_dataSAP.getSAPInfo();
}

FTControlChannelManager& P3ReplicationGroup::getControlChannelManager() {
    return m_clientControlManager;
}

bool P3ReplicationGroup::isThisPeerPrimary() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    UUIDPtr uuid;
    bool ret = getPrimaryUUID(uuid);
    if (!ret) {

        return false;
    }
    return this->isThisPeer(uuid);
}

bool P3ReplicationGroup::getPrimaryUUID(UUIDPtr& uuid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    uuid = m_primaryPeer->getUUID();
    return true;
}

bool P3ReplicationGroup::isPeerPrimary(UUIDPtr& uuid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    return m_primaryPeer->getUUID()->isEqual(uuid);
}

bool P3ReplicationGroup::isPeerReplica(UUIDPtr& uuid) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    if (m_replicas.size() == 0) {
        return false;
    }
    list<OverlayPeerInfoPtr>::iterator iter = m_replicas.begin();
    if ((*iter)->getUUID()->isEqual(uuid)) {
        return true;
    }
    iter++;
    while (iter != m_replicas.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            return true;
        }
        iter++;
    }
    //Should not reach here
    return true;
}

int P3ReplicationGroup::handleAddReplicaPacket(FTControlSvcHandler* svcHandler, FTAddReplicaPacket* packet) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::handleAddReplicaPacket ADD REPLICA PID=%s\n"), packet->getPeerInfo()->getUUID()->toString().c_str()));
    bool acceptance = this->addReplica(packet->getPeerInfo());
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::handleAddReplicaPacket after acceptance ADD REPLICA PID=%s\n"), packet->getPeerInfo()->getUUID()->toString().c_str()));
    this->printMembers();
    FTAddReplicaReplyPacket* replyPacket =
            new FTAddReplicaReplyPacket(0, packet->getPacketHeader()->getPacketID(), acceptance);
    svcHandler->sendPacket(replyPacket, 0);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::handleAddReplicaPacket Finsihed ADD REPLICA PID=%s\n"), packet->getPeerInfo()->getUUID()->toString().c_str()));
    //TODODODODODO
    return 0;
}

int P3ReplicationGroup::handleRemoveReplicaPacket(FTControlSvcHandler* svcHandler, FTRemoveReplicaPacket* packet) {
    bool acceptance = this->removeReplica(packet->getUUID());
    FTRemoveReplicaReplyPacket* replyPacket =
            new FTRemoveReplicaReplyPacket(0, packet->getPacketHeader()->getPacketID(), acceptance);
    svcHandler->sendPacket(replyPacket, 0);
    return 0;
}

bool P3ReplicationGroup::fireOnChangeToPrimary() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock, false);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::fireOnChangeToPrimary - Peer is now PRIMARY!\n")));
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::fireOnChangeToPrimary - service(%p)!\n"), getService().get()));
    bool serviceChangeStatus = getService()->changeToPrimaryRole();
    return serviceChangeStatus;
}

void P3ReplicationGroup::onPeerClosure(UUIDPtr& uuid) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, m_lock);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure uuid=%s\n"), uuid->toString().c_str()));
    bool isPrimary = this->isPeerPrimary(uuid);


    if (isPrimary) {
        if (m_replicas.size() == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - PRIMARY CLOSED with no replicas available?\n")));
            this->close();
            return;
        }
        this->m_primaryPeer = m_replicas.front();
        m_replicas.pop_front();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - PRIMARY CLOSED\n")));
        this->printMembers();
        if (this->isThisPeerPrimary()) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - NEW PRIMARY\n")));
            if (!fireOnChangeToPrimary()) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - failed to change service mode!n")));
                this->close();
            }
            UUIDPtr iid;
            this->m_svcPtr->getIID(iid);
            UUIDPtr currentReplicaIID(iid);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - NEW PRIMARY - Change IID from (%s) to (%s)\n"),
                    currentReplicaIID->toString().c_str(), m_groupUUID->toString().c_str()));
            m_ft->getOverlay()->getRuntime()->changeIIDOfService(m_sid, currentReplicaIID, m_groupUUID); //m_iid);

            return;
        }
    } else {
        this->removeReplica(uuid); //updates the group, a new primary is implicit elected
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3ReplicationGroup::onPeerClosure - REPLICA CLOSED\n")));
        this->printMembers();
    }
}

int P3ReplicationGroup::handleReplicationPacket(FTDataSvcHandler* svcDataHandler, FTDataReplicationPacket* replicationPacket) {
    if (!this->isThisPeerPrimary()) {
        m_svcPtr->onReplication(replicationPacket->getPayload()->duplicate());
    }
    return 0;
}

void P3ReplicationGroup::replicate(ACE_Message_Block* payload) throw (ServiceException&) {
    m_clientDataManager.replicate(payload);
    ACE_Message_Block::release(payload);
}