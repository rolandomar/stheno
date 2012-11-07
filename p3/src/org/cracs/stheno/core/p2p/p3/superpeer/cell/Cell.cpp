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
 * File:   Cell.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 30, 2010, 12:31 PM
 */

#include "Cell.h"

#include <stheno/core/p2p/p3/P3.h>
#include <stheno/core/p2p/p3/superpeer/SuperPeer.h>
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/ParentCellClientHandler.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/CellDiscovery.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellSvcHandler.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellFollowerSvcHandler.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellClientHandler.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellAcceptor.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellFollowerAcceptor.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellGroup.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/p2p/p3/superpeer/cell/NotifyThread.h>
#include <euryale/net/NetworkUtils.h>
#include <euryale/common/uuid/PeerLocation.h>
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>

#include <stheno/core/p2p/p3/superpeer/discovery/SuperPeerDiscovery.h>

#include <stheno/core/p2p/p3/mesh/InfoUpdateFactory.h>

#include <stheno/common/TraceRuntime.h>

Cell::Cell(P3Mesh* listener,QoSManagerInterface* qosManager) :
m_started(false), m_listener(listener), m_discovery(0), m_parentChannel(0),m_cellGroup(0),m_cellFollowerGroup(0),m_closed(false),
m_cellTree(0),m_rootDiscovery(0),m_notify(0),m_qosManager(qosManager){
    m_debugCell = RuntimeConfiguration::isClassDebugged("Cell");
     try {
        if (!P3Configuration::instance()->lookupValue("CELL_ROOT_DISCOVERY_PORT", m_startAllocationPort)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
     } catch (libconfig::FileIOException& ex) {
        throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
     }
     m_startAllocationPort++;
    //m_discovery = new CellDiscovery(CellID::ROOT_CELL_ID,m_listener->getUUID(),this);
    //m_discovery = new CellDiscovery();
}

Cell::~Cell() {
    if(m_rootDiscovery!=0){
        delete m_rootDiscovery;
    }
    if (m_discovery != 0) {
        delete m_discovery;
    }
    if (!m_parentChannel.null()) {
        m_parentChannel->close();
        //delete m_parentChannel;
    }
    if(m_cellFollowerGroup != 0){
        m_cellFollowerGroup->close();
        delete m_cellFollowerGroup;
    }
    
    if(m_cellGroup != 0){
        m_cellGroup->close();
        delete m_cellGroup;
    }

    if(m_cellTree != 0){
        delete m_cellTree;
        m_cellTree = 0;
    }
}

QoSManagerInterface* Cell::getQoSManager(){
    return m_qosManager;
}

void Cell::open(CellReply* info,CellDiscovery* discovery){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    //ROOT
    //if(info == 0 || info->getCell()->getCellID()->isRoot()) {
    if(info != 0){
        if(m_debugCell){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::open() - PID(%s) parentDiscSAP(%@)\n"),runtimeUUID->toString().c_str(),
            info->getDiscoverySAPInfo().get()));
        }
    }else{
        if(m_debugCell){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::open() - PID(%s)\n"),runtimeUUID->toString().c_str()));
        }
    }
    if(info == 0 || info->getParentCell().null()) {
        /*if(!m_parentDiscoverySAP.null()){
            m_parentDiscoverySAP.release();
        }*/
        if(m_debugCell){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::open() - root\n")));
        }
        //CellIDPtr rootNode(CellID::convertLvlsToCellID());

        if(info !=0){
            m_cellID = info->getCell()->getCellID();
            m_fid = UUIDPtr(new UUID(*m_cellID.get()));
        }else{
            CellIDPtr rootNode(new CellID(CellID::generateUUID()));
            m_cellID = rootNode;
            m_fid = UUIDPtr(new UUID(*m_cellID.get()));
        }
        m_parentCellID = CellID::INVALID_CELL_ID;
        //m_cellID = CellID::ROOT_CELL_ID;
        //m_parentCellID = CellID::ROOT_CELL_ID;
        //ROOT - COORD.
        if(info == 0 || !info->hasCoordinator()){
            CellAddrPtr parentCell;
            EndpointPtr parentCoordinator;
            SAPInfoPtr parentdiscoverySAP;
            SAPInfoPtr parentMeshSAP;
            SAPInfoPtr parentFTSAP;
            UUIDPtr parentUUID;            
            m_coordinator = true;
            joinCoordinator(parentUUID,
                            parentCell,
                            parentCoordinator,                        
                        parentdiscoverySAP,
                    parentMeshSAP,
                    parentFTSAP);
            m_started = true;
            m_rebinding = false;
            m_discovery = discovery;            
            m_discovery->attach(this);
            delete info;
            
            TraceRuntimeSingleton::instance()->logBindToMesh(runtimeUUID,m_fid);
            return;
        }//ROOT - NON_COORD.
        else{
            m_coordinator = false;
            joinNonCoordinator(info->getCoordinatorUUID(),info->getCoordinatorAddr());
            m_parentDiscoverySAP = info->getDiscoverySAPInfo();
            
            m_started = true;
            m_rebinding = false;
            m_discovery = discovery;            
            m_discovery->attach(this);
            delete info;
            TraceRuntimeSingleton::instance()->logBindToMesh(runtimeUUID,m_fid);
            return;
        }
    }//NOT_ROOT
    else {
        //NOT_ROOT - COORD.
        m_cellID = info->getCell()->getCellID();
        m_fid = UUIDPtr(new UUID(*m_cellID.get()));
        if(m_debugCell){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::open() - not root join - CellID=%s\n"),m_cellID->toString().c_str()));
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Not root join - UUID=%s\n",info->getCoordinatorUUID()->toString().c_str());
        m_parentCellID = info->getParentCell()->getCellID();
        //m_parentDiscoverySAP = info->getDiscoverySAPInfo();
        if(!info->hasCoordinator()){
            m_coordinator = true;
            joinCoordinator(
                    info->getParentUUID(),
                    info->getParentCell(),
                    info->getParentCoordinatorAddr(),
                    info->getDiscoverySAPInfo(),
                    info->getMeshSAPInfo(),
                    info->getFTSAPInfo()
                    );                        
            m_started = true;
            m_rebinding = false;
            //TODO: change this
            QoSManagerInterface* qosManager = getQoSManager();
            UUIDPtr listenerUUID;
            m_listener->getUUID(listenerUUID);
            m_discovery = new CellDiscovery(listenerUUID,qosManager);
            m_discovery->open(info->getCell(),this);            
            UUIDPtr pFID(info->getParentCell()->getCellID()->duplicate());            
            TraceRuntimeSingleton::instance()->logBindToMesh(runtimeUUID,m_fid,info->getParentUUID(),pFID);
            delete info;            
            discovery->close();            
            delete discovery;                        
            return;
        }//NOT_ROOT - NON_COORD.
        else {
            m_coordinator = false;
            joinNonCoordinator(info->getCoordinatorUUID(), info->getCoordinatorAddr());
            m_parentDiscoverySAP = info->getDiscoverySAPInfo();
            m_started = true;
            m_rebinding = false;
            //TODO: change this
            QoSManagerInterface* qosManager = getQoSManager();
            UUIDPtr listenerUUID;
            m_listener->getUUID(listenerUUID);
            m_discovery = new CellDiscovery(listenerUUID, qosManager);
            m_discovery->open(info->getCell(), this);
            UUIDPtr pFID(info->getParentCell()->getCellID()->duplicate());
            TraceRuntimeSingleton::instance()->logBindToMesh(runtimeUUID, m_fid, info->getParentUUID(), pFID);
            delete info;
            discovery->close();
            delete discovery;
            return;
        }
    }
}

void Cell::close() throw (CellException&) {
    if (m_closed) {
        return;
    }
    m_closed = true;
    m_started = false;
    m_rebinding = false;
    if (m_discovery != 0) {
        m_discovery->close();
    }
    m_acceptor->close();
}

void Cell::attach(CellDiscovery* discovery) throw (ServiceException&) {
    if (m_discovery != 0) {
        throw ServiceException("ERROR: Cell::attach() - cell already has a discovery channel!");
    }
    m_discovery = discovery;
    m_discovery->attach(this);
}

CellReply* Cell::requestCell() throw (ServiceException&) {
    //TODO: change this
    QoSManagerInterface* qosManager = getQoSManager();
    UUIDPtr listenerUUID;
    m_listener->getUUID(listenerUUID);
    CellDiscovery* discovery = new CellDiscovery(listenerUUID, qosManager);
    discovery->open();
    CellReply* cellReply = discovery->requestCell(CellID::INVALID_CELL_ID_UUIDPTR);
    discovery->close();
    return cellReply;
}

void Cell::joinNonCoordinator(UUIDPtr& coordinatorUUID, EndpointPtr& coordinator) throw (ServiceException&) {
    //if(m_debugCell){
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Not coordinator!\n")));
    //}
    try {
        ACE_Message_Block* mb = 0;
        //join_i(endpointCoordinator, mb);
        //m_started = true;
        //m_coordinator = false;        
        //pre-create this
        m_cellGroup = new CellGroup(this);
        UUIDPtr runtimeUUID;
        this->getPID(runtimeUUID);
        m_cellFollowerGroup = new CellGroup(runtimeUUID, m_cellID);
        m_childrenGroup = new CellGroup(this);
        m_leafGroup = new CellGroup(this);
        //new addition
        m_parentUUID = coordinatorUUID;
        createSAP();
        ACE_Connector<CellClientHandler, ACE_SOCK_Connector> connector;

        //m_client = new CellClientHandler(coordinatorUUID, this, true, false, 0, 0, 0, this);
        ThreadPerConnection *tpc = new ThreadPerConnection();
        //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
        //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
        ExecutionModelPtr* tpcPrt = new ExecutionModelPtr(tpc);

        CellClientHandler* client = new CellClientHandler(coordinatorUUID, this, false, false, tpcPrt, 0, 0, 0);

        CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
        CPUReservation* reserve = 0;
        if (getQoSManager() != 0) {
            reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
        }
        (tpc)->bind(client);
        (tpc)->open(reserve, cpuQoS);
        //tpc->bind(m_client);
        connector.reactor(tpc->getResources()->getReactor());

        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connecting...(%s)!\n"),
                    coordinator->toString().c_str()));
        }
        if (connector.connect(client, coordinator->getAddr()) == -1) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                    ACE_TEXT("(%T)connect failed:")));
            client->close();
            delete client;
            m_client.reset(0);

            throw ServiceException("Error connecting to coordinator!");
        } else {
            m_client.reset(client);
            if (m_debugCell) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connect OK!\n")));
            }
            if (client->asynchronous(true, false) == -1) {
                client->close();
                //delete client; This is controlled bu the smart Ptr!
                m_client.reset(0);
                throw ServiceException("Error connecting to coordinator!");
            }
            bool joinRet = m_client->joinMesh(mb);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - join ret(%d)\n"), joinRet));


            if (!joinRet || mb == 0) {
                ACE_Message_Block::release(mb);
                client->close();
                //delete client; This is controlled bu the smart Ptr!
                m_client.reset(0);
                throw ServiceException("Error connecting to coordinator!");
            } else {
                ByteInputStream is(mb);
                if (m_cellTree != 0) {
                    delete m_cellTree;
                    m_cellTree = 0;
                }
                m_cellTree = new CellTree(is);
                //String treeStr(m_cellTree->toStringPerLevel());
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - tree\n%s\n"), treeStr.c_str()));
                //cleanup
                ACE_Message_Block::release(mb);
                if (client->setCloseListener(this) == -1) {
                    client->close();
                    //delete client; This is controlled bu the smart Ptr!
                    m_client.reset(0);
                    throw ServiceException("Error connecting to coordinator!");
                }
            }
        }
        /*ByteInputStream is(mb);
        if (m_cellTree != 0) {
            delete m_cellTree;
        }
        m_cellTree = new CellTree(is);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - tree\n%s\n"), m_cellTree->toStringPerLevel().c_str()));
        //cleanup
        ACE_Message_Block::release(mb);*/

    } catch (CellException& joinEx) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell:joinNonCoordinator() - ex1\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
}

void Cell::joinCoordinator(UUIDPtr& parentUUID,
        CellAddrPtr& parentCellAddr,
        EndpointPtr& parentCoordinator,
        SAPInfoPtr& parentDiscoverySAP,
        SAPInfoPtr& parentMeshSAP,
        SAPInfoPtr& parentFTSAP) throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::joinCoordinator() - Coordinator!\n")));
    try {
        //join_i();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::joinCoordinator() - NO COORDINATOR\n")));
        //m_started = true;
        //m_coordinator = true;
        m_cellGroup = new CellGroup(this);
        UUIDPtr runtimeUUID;
        this->getPID(runtimeUUID);
        m_cellFollowerGroup = new CellGroup(runtimeUUID, m_cellID);
        m_childrenGroup = new CellGroup(this);
        m_leafGroup = new CellGroup(this);
        createSAP();


        m_cellTree = new CellTree(m_cellID);
        int type = P3PeerInfo::SUPERPEER_COORD;
        UUIDPtr fid(m_cellID->duplicate());
        SAPInfoPtr meshSAP;
        UUIDPtr listenerUUID;
        m_listener->getUUID(listenerUUID);
        this->getMeshSAPInfo(listenerUUID, meshSAP);
        SAPInfoPtr discoverySAP;
        this->getDiscoverySAPInfo(listenerUUID, discoverySAP);
        SAPInfoPtr ftSAP;
        this->getFTSAPInfo(listenerUUID, ftSAP);
        P3PeerInfo* pi = new P3PeerInfo(type,
                runtimeUUID,
                fid,
                m_acceptor->getEndpoint(),
                meshSAP, discoverySAP, ftSAP);
        m_cellTree->addPeer(pi);
        //String treeStr(m_cellTree->toStringPerLevel());
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::joinCoordinator() - tree\n%s\n"), treeStr.c_str()));
        //NOT_ROOT
        if (!parentCoordinator.null()) {
            connecToParentCoordinator(parentUUID,
                    parentCellAddr,
                    parentCoordinator,
                    parentDiscoverySAP,
                    parentMeshSAP,
                    parentFTSAP);
        } else {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::joinCoordinator() - no parent endpoint info?\n")));
        }

    } catch (CellException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::joinCoordinator() ex2\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
    if (m_notify == 0) {
        m_notify = new NotifyThread(this);
        if (m_notify->activate() == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::joinCoordinator() notify thread errror!\n")));
            perror("failed to active notify thread:");
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::joinCoordinator() notify thread ok!\n")));
        }
    }
    //m_discovery->asynchronous();
}

bool Cell::connecToParentCoordinator(UUIDPtr& parentUUID,
        CellAddrPtr& parentCellAddr,
        EndpointPtr& parentCoordinator,
        SAPInfoPtr& parentDiscoverySAP,
        SAPInfoPtr& parentMeshSAP,
        SAPInfoPtr& parentFTSAP) throw (ServiceException&) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), false);
    ACE_Connector<ParentCellClientHandler, ACE_SOCK_Connector> connector;
    m_parentUUID = parentUUID;
    m_parentDiscoverySAP = parentDiscoverySAP;
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::connecToParentCoordinator() parentDiscSAP(%@)\n"), parentDiscoverySAP.get()));
    }
    m_parentMeshSAP = parentMeshSAP;
    m_parentFTSAP = parentFTSAP;

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new ExecutionModelPtr(tpc);
    //ParentCellClientHandler* parentChannel = new ParentCellClientHandler(parentUUID, parentCellAddr->getCellID(), this, true, false, 0, 0, 0, this);
    ParentCellClientHandler* parentChannel = new ParentCellClientHandler(
            parentUUID,
            parentCellAddr->getCellID(),
            this,
            false,
            false,
            tpcPrt,
            0, 0, 0);


    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    (tpc)->bind(parentChannel);
    (tpc)->open(reserve, cpuQoS);
    //tpc->bind(m_parentChannel);
    connector.reactor(tpc->getResources()->getReactor());


    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::join() - Connecting to parent coordinator...(%s)!\n"), parentCoordinator->toString().c_str()));
    if (connector.connect(parentChannel, parentCoordinator->getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)ERROR Cell:join Error on parent connect!\n")));
        parentChannel->close();
        m_parentChannel.reset(0);
        delete parentChannel;
        throw ServiceException("Failed to connect to parent coordinator!");
    } else {
        m_parentChannel.reset(parentChannel);
        if (m_parentChannel->asynchronous(true, false) == -1) {
            parentChannel->close();
            //delete parentChannel;
            m_parentChannel.reset(0);
            return false;
        }

        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::rebindToParentCoordinator() - Connect OK!\n")));
        }
        //ACE_Message_Block* mb = 0;
        bool joinRet = m_parentChannel->joinMesh();
        if (!joinRet || m_parentChannel->setCloseListener(this) == -1) {
            parentChannel->close();
            //delete parentChannel;
            m_parentChannel.reset(0);
            return false;
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::join() - join(%d)\n"), joinRet));
        return joinRet;
    }

}

void Cell::createSAP() throw (ServiceException&) {
    m_acceptor = new CellAcceptor(this, m_cellGroup, m_cellFollowerGroup, m_childrenGroup, m_leafGroup);
    m_followerAcceptor = new CellFollowerAcceptor(this);
    String itf;
    String ip;
    UInt maxBinds = 0;
    //Short startPort = 0;
    try {
        if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        if (!P3Configuration::instance()->lookupValue("MAX_BINDS", maxBinds)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        /*if (!P3Configuration::instance()->lookupValue("CELL_COORDINATOR_BASE_PORT", startPort)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }*/
        ACE_INET_Addr* addr = NetworkUtils::get_ip_interface(itf.c_str());
        if (addr == 0) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - interface: %s IP=%s\n"), itf.c_str(), addr->get_host_addr()));
        ip = addr->get_host_addr();
        delete addr;
    } catch (libconfig::FileIOException& ex) {
        throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - find a suitable port\n")));
    bool bindOk = false;
    for (int i = 0; i < maxBinds + 1000; i++) {
        try {
            EndpointPtr endpoint(
                    //new Endpoint(Endpoint::TCP, new String("127.0.0.1"),
                    new Endpoint(Endpoint::TCP, ip,
                    NetworkUtils::randomPort(), new QoSEndpoint(QoS::SRT, 50)));
            if (m_acceptor->open(endpoint) == 0) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - acceptor ok\n")));
                bindOk = true;
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - ********Endpoint(%s)******\n"),
                //endpoint->toString().c_str()));
                break;
            } else {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::join() - acceptor nok\n")));
                //startPort++;
            }
        } catch (EndpointException& ex) {
        }
    }

    if (!bindOk) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::createSAP() - bind nok\n")));
        throw CellException(CellException::BIND_FAILED);
    }

    bindOk = false;
    for (int i = 0; i < 1000; i++) {
        EndpointPtr endpoint(
                //new Endpoint(Endpoint::TCP, new String("127.0.0.1"),
                new Endpoint(Endpoint::TCP, ip,
                NetworkUtils::randomPort(), new QoSEndpoint(QoS::SRT, 50)));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - ********Endpoint(%@)******\n"),
                endpoint.get()));
        if (m_followerAcceptor->open(endpoint) == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - Follower acceptor ok\n")));
            bindOk = true;
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - ********Endpoint(%s)******\n"),
            //endpoint->toString().c_str()));
            break;
        }
    }

    if (!bindOk) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::createSAP() - bind nok\n")));
        throw CellException(CellException::BIND_FAILED);
    }


    //TODO
    /*if (this->activate() != 0) {
        close();
        throw CellException(CellException::FAILED_TO_LAUNCH_THREAD);
    }*/

}

void Cell::handleNetworkPartition(CellNotifyPacket* packet) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    CellIDPtr runtimeCellID;
    this->getCellID(runtimeCellID);
    if (*(runtimeCellID.get()) != *packet->getPacketHeader()->m_srcFID) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleNetworkPartition() - \n***********THE MOST INTERESTING LINE OF CODE!*********\n")));
        //negociate
        delete packet;
        //m_listener->onCellShutdown();
        fireRebind();
        return;
    }
    delete packet;

}

void Cell::handleCoordinatorRequest(CellDiscovery* discovery, RequestCoordinatorPacket* packet) {
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - before lock\n")));
    }*/
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest()\n")));
    }*/
    list<P3PeerInfoPtr> l;
    m_cellTree->getCoordinators(l);
    int size = l.size();
    if (size == 0) {
        delete packet;
        return;
    }
    int num = (size - 1) % ACE_OS::rand();
    list<P3PeerInfoPtr>::iterator iter = l.begin();
    for (int i = 0; i < num; i++) {
        iter++;
    }
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - size=%d num=%d\n"), size, num));
    }*/
    UUIDPtr cellID(m_cellID->duplicate());
    P3PeerInfoPtr coord = *iter;
    if (coord.null()) {
        /*if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - null cordinator?\n")));
        }*/
        delete packet;
        return;
    }
    SAPInfoPtr meshPtr = coord->getMeshSAP();
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - vedore discovery\n")));
    }*/
    SAPInfoPtr discoveryPtr = coord->getDiscoverySAP(); //m_listener->getOverlay_i()->getDiscovery()->getSAP();//stupid
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - after discovery\n")));
    }*/
    SAPInfoPtr ftPtr = coord->getFTSAP();
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRequest() - PID=%s discoverySAP=%@\n"),
                coord->getPID()->toString().c_str(),
                coord->getDiscoverySAP().get()
                ));
    }*/
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);

    RequestCoordinatorReplyPacket *reply = new
            RequestCoordinatorReplyPacket(
            runtimeUUID,
            cellID,
            packet->getPacketHeader()->m_srcPID,
            packet->getPacketHeader()->m_srcFID,
            packet->getPacketHeader()->getPacketID(),
            coord->getPID(),
            coord->getFID(),
            meshPtr,
            discoveryPtr,
            ftPtr,
            coord->getCellEndpoint());
    //this->getSAP()); //stupid    
    discovery->sendPacket(reply, 0);
    //cleanup
    delete reply;
    delete packet;
}

void Cell::handleCellRequest(CellDiscovery* discovery, RequestCellPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - before lock\n")));
    }
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest()\n")));
    }
    //if (m_cellID->isRoot()) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Cell::handleCellRequest(): getCellID=%s\n", getCellID()->toString().c_str());
    if (m_coordinator) {
        Short port = -1;
        String itf;
        String ip;
        CellTreeNode* node = 0;
        if (packet->getPacketHeader()->getSrcFID()->isEqual(CellID::INVALID_CELL_ID_UUIDPTR)) {

            node = m_cellTree->getAvailableNode(P3PeerInfo::SUPERPEER);
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - node=%@ rootNode=%@\n"), node, m_cellTree->getRootNode()));
            if (node == 0) {
                node = m_cellTree->createCell();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - new cell(%@,%s)\n"), node, node->getCellID()->toString().c_str()));
                port = this->m_startAllocationPort++;
                node->setPort(port);
            }
            UUIDPtr cellID(new CellID(node->getCellID().get()));

            //EndpointPtr& endpoint = m_acceptor->getEndpoint();
            //UUIDPtr cellID(new CellID(getCellID().get()));
            try {

                if (!P3Configuration::instance()->lookupValue("CELL_MULTICAST_INTERFACE", itf)) {
                    throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
                }
                if (!P3Configuration::instance()->lookupValue("CELL_ROOT_DISCOVERY_IP", ip)) {
                    throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
                }
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscovery : Addr=(%s,%s,%d)\n", itf.c_str(), ip.c_str(), port);
            } catch (libconfig::FileIOException& ex) {
                delete packet;
                return;
            }
            //if (node->getCellID()->isEqual(*CellID::ROOT_CELL_ID)) {
            if (isRoot() && node == m_cellTree->getRootNode()) {
                try {
                    if (!P3Configuration::instance()->lookupValue("CELL_ROOT_DISCOVERY_PORT", port)) {
                        throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
                    }
                } catch (libconfig::FileIOException& ex) {
                    delete packet;
                    return;
                }
            }


            SAPInfoPtr discoverySAP;
            SAPInfoPtr meshSAP;
            SAPInfoPtr ftSAP;

            CellAddr* cellAddr_ = new CellAddr(node->getCellID(), itf, ip, port);
            CellAddrPtr cellAddr(cellAddr_);
            P3PeerInfoPtr coord;
            node->getCoordinator(coord);
            UUIDPtr coordUUID;
            EndpointPtr coordEndpoint;
            if (coord != 0) {
                if (m_debugCell) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - \nCOORDINATOR PID(%s)\n"), coord->getPID()->toString().c_str()));
                }
                coordUUID = coord->getPID();
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - coord(%s)\n"), coord->getCellEndpoint()->toString().c_str()));
                coordEndpoint = coord->getCellEndpoint();
                //discoverySAP = m_listener->getOverlay_i()->getP3Discovery()->getSAP();
                discoverySAP = coord->getDiscoverySAP();

                meshSAP = coord->getMeshSAP();
                //FT missing
                ftSAP = coord->getFTSAP();
            } else {
                //coordinator
                if (node->getParent() != 0) {
                    P3PeerInfoPtr nodeCoord;
                    node->getParent()->getCoordinator(nodeCoord);
                    //parent's coordinator
                    discoverySAP = nodeCoord->getDiscoverySAP();
                    meshSAP = nodeCoord->getMeshSAP();
                    ftSAP = nodeCoord->getFTSAP();
                }
            }


            CellAddrPtr parentCellAddr;
            UUIDPtr parentUUID;
            EndpointPtr parentEndpoint;

            if (node->getParent() != 0) {
                CellAddr* cellAddr_ = new CellAddr(node->getParent()->getCellID(), itf, ip, port);
                CellAddrPtr cellAddr(cellAddr_);
                parentCellAddr = cellAddr;
                P3PeerInfoPtr coord;
                node->getParent()->getCoordinator(coord);
                parentUUID = coord->getPID();
                parentEndpoint = coord->getCellEndpoint();
            }
            /*if (m_debugCell) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - \nreply(%@,%@,%@,%@,%@,%@,%@,%@)\n"),
                        cellAddr.get(),
                        coordUUID.get(),
                        coordEndpoint.get(),
                        parentCellAddr.get(),
                        parentEndpoint.get(),
                        discoverySAP.get(),
                        meshSAP.get(),
                        ftSAP.get()
                        ));
            }*/
            UUIDPtr runtimeUUID;
            this->getPID(runtimeUUID);

            RequestCellReplyPacket *reply = new
                    RequestCellReplyPacket(runtimeUUID,
                    cellID,
                    packet->getPacketHeader()->m_srcPID,
                    packet->getPacketHeader()->m_srcFID,
                    packet->getPacketHeader()->getPacketID(),
                    cellAddr, //cell addr (mcast,port)
                    coordUUID, //uuid coordinator
                    coordEndpoint, //coordinator cell mesh access or
                    parentCellAddr, //cell addr (mcast,port)
                    parentUUID, //parent coord uui
                    parentEndpoint, //parent coordinator mesh access
                    discoverySAP,
                    meshSAP,
                    ftSAP
                    );
            //((P3*)m_listener->getOverlay())->getP3Discovery()->getSAP());
            discovery->sendPacket(reply, 0);
            delete reply;
        } else {
            node = m_cellTree->getCellTreeNode(packet->getPacketHeader()->getSrcFID());
            if (node == 0) {
                node = m_cellTree->getParentAvailableLocation();
            } else {
                node = node->getParent();
            }
            CellAddr* cellAddr_ = new CellAddr(node->getCellID(), itf, ip, port);
            CellAddrPtr cellAddr(cellAddr_);
            P3PeerInfoPtr coord;
            node->getCoordinator(coord);
            UUIDPtr coordUUID;
            EndpointPtr coordEndpoint;
            if (coord != 0) {
                coordUUID = coord->getPID();
                coordEndpoint = coord->getCellEndpoint();
            }
            CellAddrPtr parentCellAddr;
            UUIDPtr parentUUID;
            EndpointPtr parentEndpoint;
            SAPInfoPtr discoverySAP;
            SAPInfoPtr meshSAP;
            SAPInfoPtr ftSAP;

            if (node != 0) {
                CellAddr* cellAddr_ = new CellAddr(node->getCellID(), itf, ip, port);
                CellAddrPtr cellAddr(cellAddr_);
                parentCellAddr = cellAddr;
                P3PeerInfoPtr coord;
                node->getCoordinator(coord);
                parentUUID = coord->getPID();
                parentEndpoint = coord->getCellEndpoint();
                discoverySAP = coord->getDiscoverySAP();
                meshSAP = coord->getMeshSAP();
                ftSAP = coord->getFTSAP();
                //missing FT
            }
            UUIDPtr cellID(new CellID(node->getCellID().get()));
            /*if (m_debugCell) {
                SAPInfoPtr sapInfo;
                m_listener->getOverlay_i()->getP3Discovery()->getSAP(sapInfo);
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - \nreply(%@,%@,%@,%@,%@,%@,%@,%@,%@)\n"),
                        cellAddr.get(),
                        coordUUID.get(),
                        coordEndpoint.get(),
                        parentCellAddr.get(),
                        parentEndpoint.get(),
                        sapInfo.get(),
                        discoverySAP.get(),
                        meshSAP.get(),
                        ftSAP.get()
                        ));
            }*/
            if (!coordUUID.null()) {
                /*if (m_debugCell) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - REPLY COORD(%s,%s)\n"),
                            coordUUID->toString().c_str(),
                            coordEndpoint->toString().c_str()));
                }*/
            }
            if (!parentCellAddr.null()) {
                /*if (m_debugCell) {
                      ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCellRequest() - REPLY parentCellAddr(%s,%s)\n"),
                              parentCellAddr->getCellID()->toString().c_str(),
                              parentEndpoint->toString().c_str()));
                  }*/
            }
            UUIDPtr runtimeUUID;
            this->getPID(runtimeUUID);

            RequestCellReplyPacket *reply = new
                    RequestCellReplyPacket(runtimeUUID,
                    cellID,
                    packet->getPacketHeader()->m_srcPID,
                    packet->getPacketHeader()->m_srcFID,
                    packet->getPacketHeader()->getPacketID(),
                    cellAddr,
                    coordUUID,
                    coordEndpoint,
                    parentCellAddr,
                    parentUUID,
                    parentEndpoint,
                    discoverySAP,
                    meshSAP,
                    ftSAP
                    );
            //((P3*)m_listener->getOverlay())->getP3Discovery()->getSAP());
            discovery->sendPacket(reply, 0);
            delete reply;

        }
    } else {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Cell:handleCellRequest() - not coordinator, discarding packet\n")));
    }
    //} else {
    //Discard
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Error: Cell::handleCellRequest() - not root and received a cell request\n")));
    //}
    delete packet;

}


//from children

int Cell::handleRebindMesh(CellSvcHandler* channel, RebindMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() -before lock\n")));
    }

    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() \n")));
    }

    ACE_Message_Block* mb = packet->deattachState();
    ACE_Message_Block* mb2 = 0;
    ACE_Message_Block* mb3 = 0;
    if (mb != 0) {
        mb2 = mb->duplicate();
        mb3 = mb->duplicate();
    }
    ByteInputStream is(mb);
    CellTree* tree = new CellTree(is);
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() - %s \n"), tree->toString().c_str()));
    }

    int ret = m_cellTree->addSubtree(tree, packet->getParentCellID());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() - %s add=%d\n!\n"), packet->getParentCellID()->toString().c_str(), ret));
    }
    bool accepted = false;
    if (ret == 0) {
        accepted = true;
    }
    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    ace_mon.release();

    if (accepted && !parentHandler.null()) {
        if (!parentHandler->rebindSiblingPeer(
                P3PeerInfo::SUPERPEER,
                packet->getUUID(),
                packet->getCellID(),
                packet->getParentCellID(),
                packet->getEndpoint(),
                packet->getDiscoverySAP(),
                packet->getMeshSAP(),
                packet->getFTSAP(),
                mb2)) {
            accepted = false;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell:handleRebindMesh() - failed to add to parent!\n")));
            //must restart bind process and find a suitable parent
        }
    }
    UUIDPtr cellID(m_cellID->duplicate());
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);
    SthenoPacket* rebindReplyPacket =
            new RebindMeshReplyPacket(
            runtimeUUID,
            cellID,
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            accepted);
    ACE_Time_Value* timeout = 0;
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() - send to group pid=%s!\n"), packet->getUUID()->toString().c_str()));
    }
    //ace_mon.release();
    m_cellFollowerGroup->rebindPeer(packet->getType(), packet->getUUID(), packet->getCellID(), packet->getParentCellID(),
            packet->getEndpoint(),
            packet->getDiscoverySAP(),
            packet->getMeshSAP(),
            packet->getFTSAP(),
            mb3);
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() - reply to child!\n")));
    }
    channel->sendPacket(rebindReplyPacket, timeout);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleRebindMesh() - Tree\n%s\n"), m_cellTree->toString().c_str()));
    delete rebindReplyPacket;
    delete packet;
    ACE_Message_Block::release(mb);
    //ACE_Message_Block::release(mb2);
    //ACE_Message_Block::release(mb3);
    return 0;
}

int Cell::handleJoinMesh(CellSvcHandler* channel, JoinMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() before lock\n")));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    UUIDPtr cellID(m_cellID->duplicate());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh()\n")));
    }
    UUIDPtr fid(packet->getCellID()->duplicate());

    SAPInfoPtr discoverySAP = packet->getDiscoverySAP();
    SAPInfoPtr meshSAP = packet->getMeshSAP();
    SAPInfoPtr ftSAP = packet->getFTSAP();
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - discoverySAP=%@\n"),
                discoverySAP.get()
                ));
    }
    /*int maxWait = 3;
    while (m_rebinding && maxWait > 0) {
        SleepHelper::sleep(50);
        maxWait--;
    }*/


    P3PeerInfo* pi = new P3PeerInfo(packet->getType(), packet->getUUID(), fid,
            packet->getEndpoint(),
            meshSAP, discoverySAP, ftSAP);

    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - pid(%s) endpoint(%s)\n"),
            packet->getUUID()->toString().c_str(), packet->getEndpoint()->toString().c_str()));*/

    //int ret = m_cellTree->addPeer(pi);
    CellIDPtr parentCellIDPtr;
    if (packet->getType() == P3PeerInfo::SENSOR || packet->getType() == P3PeerInfo::MONITOR) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell:handleJoinMesh() - sensor or monitor\n")));
        parentCellIDPtr = m_cellID;
    } else {
        parentCellIDPtr = packet->getParentCellID();
    }

    int ret = 0;
    /*if (packet->getParentCellID().null()) {
        ret = m_cellTree->addPeer(pi);
    } else {
        ret = m_cellTree->addPeer(pi, parentCellIDPtr);
    }*/
    if (parentCellIDPtr.null()) {
        ret = m_cellTree->addPeer(pi);
    } else {
        ret = m_cellTree->addPeer(pi, parentCellIDPtr);
    }

    bool accepted = false;
    if (ret == 0) {
        accepted = true;
    }

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - Accepted=%d pid=%s!\n"), accepted, packet->getUUID()->toString().c_str()));
    }

    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    //String treeStr(m_cellTree->toStringPerLevel());
    ace_mon.release();

    if (accepted && !parentHandler.null()) {
        if (!parentHandler->joinSiblingPeer(
                //P3PeerInfo::SUPERPEER,
                packet->getType(),
                packet->getUUID(),
                packet->getCellID(),
                parentCellIDPtr,
                packet->getEndpoint(),
                packet->getDiscoverySAP(),
                packet->getMeshSAP(),
                packet->getFTSAP()

                )) {
            accepted = false;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell:handleJoinMesh() - \nfailed to add to parent!\n")));
            //must restart bind process and find a suitable parent
        }
    }
    //ace_mon.release();
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);

    SthenoPacket* joinReply =
            new JoinMeshReplyPacket(
            runtimeUUID,
            cellID,
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            accepted);
    ACE_Time_Value* timeout = 0;

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - send join to group pid=%s!\n"), packet->getUUID()->toString().c_str()));
    }

    m_cellFollowerGroup->joinPeer(packet->getType(), packet->getUUID(), packet->getCellID(), packet->getParentCellID(),
            packet->getEndpoint(), packet->getDiscoverySAP(), packet->getMeshSAP(), packet->getFTSAP());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - after send join!\n")));
    }
    channel->sendPacket(joinReply, timeout);
    //if (m_debugCell) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleJoinMesh() - Tree\n%s"),treeStr.c_str()));
    //}    
    delete joinReply;
    delete packet;
    return 0;
}

void Cell::getParentHandler(ParentCellClientHandlerPtr& parentHandler) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    parentHandler = m_parentChannel;
}

int Cell::leaveMesh(UUIDPtr& uuid,
        CellIDPtr& cellID) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::leaveMesh() before lock\n")));
    }

    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::leaveMesh()\n")));
    }
    Boolean status = true;
    CellTreeNode* node = m_cellTree->getCellTreeNode(cellID);
    if (node == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::leaveMesh() - no node found!\n")));
        status = false;
    } else {
        //TODO: !!!!!!!!!!!!!!!!!!!!!
        if (node->remove(uuid) != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::leaveMesh() - didn't find peer!\n")));
            status = false;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::leaveMesh() - removing subtree!\n")));
            if (node->getPeerCount() == 0) {
                m_cellTree->removeSubtree(cellID);
            }
        }

    }
    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    //String treeStr(m_cellTree->toStringPerLevel());
    ace_mon.release();

    m_cellFollowerGroup->leftPeer(uuid, cellID);
    //send to parent
    if (!parentHandler.null()) {
        if (!parentHandler->leftSiblingPeer(uuid, cellID)) {
            return -1;
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::leaveMesh() - failed to remove from parent!\n")));
        }
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::leaveMesh() - tree\n%s\n"),treeStr.c_str()));
    return 0;

}

int Cell::handleLeaveMesh(CellSvcHandler* channel, LeaveMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() before lock\n")));
    }

    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh()\n")));
    }
    Boolean status = true;
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);

    CellTreeNode* node = m_cellTree->getCellTreeNode(packet->getCellID());
    if (node == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleLeaveMesh() - no node found!\n")));
        status = false;
    } else {
        if (node->remove(packet->getUUID()) != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleLeaveMesh() - didnt find peer!\n")));
            status = false;
        } else {
            if (node->getPeerCount() == 0) {
                m_cellTree->removeSubtree(packet->getCellID());
            }
        }
    }

    UUIDPtr cellID(new CellID(runtimeCellID.get()));
    SthenoPacket* reply = new LeaveMeshReplyPacket(
            runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            status);

    ACE_Time_Value* timeout = 0;

    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    //String treeStr(m_cellTree->toStringPerLevel());
    ace_mon.release();

    //send to parent
    if (!parentHandler.null()) {
        if (!parentHandler->leftSiblingPeer(
                packet->getPacketHeader()->getSrcPID(),
                runtimeCellID
                )) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleLeaveMesh() - failed to add to parent!\n")));
        }
    }
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - send leave to group\n")));
    }
    m_cellFollowerGroup->leftPeer(packet->getUUID(), packet->getCellID());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - after leave reply to child\n")));
    }
    int ret = channel->sendPacket(reply, timeout);

    if (m_debugCell) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - tree\n%s\n"), treeStr.c_str()));
    }

    delete packet;
    delete reply;
    return ret;
}

int Cell::handleJoinCell(CellSvcHandler* channel, JoinCellPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell() before lock\n")));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell()\n")));
    }
    Boolean status = true;
    //if (m_cellID->isRoot()) {
    CellTreeNode* node = m_cellTree->getRootNode(); //m_cellTree->getAvailableNode(TreePeerInfo::SUPERPEER);
    if (!node->hasAvailableSlotsForPeer(P3PeerInfo::SUPERPEER)) {
        status = false;
    } else {
        UUIDPtr fid(node->getCellID()->duplicate()); //((*m_cellID)->duplicate());
        SAPInfoPtr meshSAP = packet->getMeshSAP();
        SAPInfoPtr discoverySAP = packet->getDiscoverySAP();
        SAPInfoPtr ftSAP = packet->getFTSAP();
        /*if (m_debugCell) {
            printf("\n\n\n\n Add peer cell %p %p\n\n\n\n", meshSAP.get(), ftSAP.get());
        }*/
        P3PeerInfo* pi = new P3PeerInfo(packet->getPacketHeader()->getSrcPID(),
                fid, packet->getEndpoint(), //m_acceptor->getEndpoint(),
                meshSAP, discoverySAP, ftSAP);
        node->add(pi);
    }

    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);

    UUIDPtr cellID(new CellID(runtimeCellID.get()));
    SthenoPacket* reply = 0;

    if (!status) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleJoinCell() - failed to add peer ID(%d)\n"),
        //packet->getPacketHeader()->getPacketID()));
        reply = new JoinCellReplyPacket(
                runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
                packet->getPacketHeader()->getSrcPID(),
                packet->getPacketHeader()->getSrcFID(),
                0,
                packet->getPacketHeader()->getPacketID());
    } else {
        StreamSize ss;
        m_cellTree->serialize(ss);
        //find size        
        ByteOutputStream os(ss.total_length());
        m_cellTree->serialize(os);
        ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
        RDR::consolidate(mb, &os.start_);
        /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ************Cell::handleJoinCell() - added peer mb_size=%l  ID(%d)\n"),
                mb->total_length(), packet->getPacketHeader()->getPacketID()));*/
        reply = new JoinCellReplyPacket(
                runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
                packet->getPacketHeader()->getSrcPID(),
                packet->getPacketHeader()->getSrcFID(),
                0,
                packet->getPacketHeader()->getPacketID(),
                mb);
        //this is now owned by the packet
        //ACE_Message_Block::release(mb);
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell() - sendind to cell uuid(%s)\n"),
    //      packet->getPacketHeader()->getSrcPID()->toString().c_str()));
    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    //String treeStr(m_cellTree->toStringPerLevel());
    ace_mon.release();


    CellIDPtr parentCellID;
    //ace_mon.release();
    CellID* srcCellID = new CellID(*(packet->getPacketHeader()->getSrcFID().get()));
    CellIDPtr srcCellIDPtr(srcCellID);
    m_cellFollowerGroup->joinPeer(P3PeerInfo::SUPERPEER, packet->getPacketHeader()->getSrcPID(),
            srcCellIDPtr, parentCellID,
            packet->getEndpoint(), packet->getDiscoverySAP(), packet->getMeshSAP(), packet->getFTSAP());


    ACE_Time_Value* timeout = 0;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell() - SENDING TO CELL PEER\n")));
    int ret = channel->sendPacket(reply, timeout);
    //send to parent
    if (!parentHandler.null()) {
        if (!parentHandler->joinSiblingPeer(
                P3PeerInfo::SUPERPEER,
                packet->getPacketHeader()->getSrcPID(),
                //cellID,
                runtimeCellID,
                parentHandler->getCellID(),
                packet->getEndpoint(),
                packet->getDiscoverySAP(), packet->getMeshSAP(), packet->getFTSAP())) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleJoinCell() - failed to add to parent!\n")));
            //must restart bind process and find a suitable parent
        }
    } else {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell() - no parent?\n")));
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleJoinCell() - tree\n%s\n"), treeStr.c_str()));
    delete packet;
    delete reply;
    return ret;
}

bool Cell::IsCoordinator() {
    return m_coordinator;

}

void Cell::getSAP(EndpointPtr& e) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    e = m_acceptor->getEndpoint();
}

void Cell::getFollowerSAP(EndpointPtr& e) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    e = m_followerAcceptor->getEndpoint();
}

int Cell::svc_i(void*) {
    while (true) {
        m_discovery->sendGroupKeepAlive();
        SleepHelper::sleep(1000);
    }
}

void Cell::handleDiscoveryPacket(CellDiscovery* discovery, SthenoPacket* packet) {
    /*if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleDiscoveryPacket() before lock\n")));
    }*/
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());

    if (m_debugCell) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleDiscoveryPacket()\n")));
    }
    if (!m_started) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleDiscoveryPacket() - not started!\n")));
        delete packet;
        return;
    }
    handleDiscoveryPacket_i(discovery, packet);
}

void Cell::handleDiscoveryPacket_i(CellDiscovery* discovery, SthenoPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleDiscoveryPacket_i() - type(%d)\n"), packet->getPacketHeader()->getType()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case REQUEST_CELL_PACKET:
        {
            RequestCellPacket* cellRequestPacket =
                    static_cast<RequestCellPacket*> (packet);
            handleCellRequest(discovery, cellRequestPacket);
            return;
        }
        case NOTIFY_PACKET:
        {
            CellNotifyPacket* notifyPacket =
                    static_cast<CellNotifyPacket*> (packet);
            handleNetworkPartition(notifyPacket);
            return;
        }

        case REQUEST_COORDINATOR_PACKET:
        {
            RequestCoordinatorPacket* requestCoordPacket =
                    static_cast<RequestCoordinatorPacket*> (packet);

            handleCoordinatorRequest(discovery, requestCoordPacket);
            return;
        }
        default:
        {
            //not know!
            //switch(packet->)
            //ok to process packet
            delete packet;
            return;
        }
    }
}

void Cell::getCellID(CellIDPtr& cellID) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    cellID = m_cellID;
}

void Cell::getFID(UUIDPtr& fid) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    fid = m_fid;
}

int Cell::getPeer(UUIDPtr& uuid, P3PeerInfoPtr& info) {
    return m_cellTree->getPeer(uuid, info);
}

int Cell::getPeerByLevel(int treeLevel, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    return m_cellTree->getPeerByLevel(treeLevel, info);
}

int Cell::getRandomPeer(P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    return m_cellTree->getRandomPeer(info);
}

void Cell::getParentCellID(CellIDPtr& parentCellID) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    parentCellID = m_parentCellID;
}

void Cell::getParentUUID(UUIDPtr& parentUUID) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    parentUUID = m_parentUUID;
}

void Cell::getPID(UUIDPtr& uuid) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    m_listener->getUUID(uuid);
}

bool Cell::rebindToNewCoordinator(UUIDPtr& uuid,
        EndpointPtr& coordinatorEndpoint) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - Not coordinator!\n")));
    }
    ACE_Time_Value start = ACE_OS::gettimeofday();
    try {
        ACE_Message_Block* mb = 0;
        ACE_Connector<CellClientHandler, ACE_SOCK_Connector> connector;

        //CellClientHandler* client = new CellClientHandler(uuid, this, true, false, 0, 0, 0, this);
        ThreadPerConnection *tpc = new ThreadPerConnection();
        //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
        //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
        ExecutionModelPtr* tpcPrt = new ExecutionModelPtr(tpc);
        CellClientHandler* client = new CellClientHandler(uuid, this, false, false, tpcPrt, 0, 0, 0);

        //tpc->open();
        CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
        CPUReservation* reserve = 0;
        if (getQoSManager() != 0) {
            reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
        }
        (tpc)->bind(client);
        (tpc)->open(reserve, cpuQoS);
        //tpc->bind(m_client);
        connector.reactor(tpc->getResources()->getReactor());

        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - Connecting...(%s)!\n"),
                    coordinatorEndpoint->toString().c_str()));
        }
        if (connector.connect(client, coordinatorEndpoint->getAddr()) == -1) {
            ACE_ERROR((LM_DEBUG, ACE_TEXT("(%T)ERROR: Cell:rebindToNewCoordinator() - Connection failed!\n")));
            client->close();
            delete client;
            m_client.reset(0);
            throw ServiceException("Error connecting to coordinator!");
        } else {
            m_client.reset(client);
            if (m_debugCell) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - Connect OK!\n")));
            }
            if (client->asynchronous(true, false) == -1) {
                client->close();
                //delete client;
                m_client.reset(0);
                throw ServiceException("Error connecting to coordinator!");
            }
            bool joinRet = m_client->joinMesh(mb);
            if (!joinRet || mb == 0) {
                ACE_Message_Block::release(mb);
                client->close();
                //delete client;
                m_client.reset(0);
                throw ServiceException("Error connecting to coordinator!");
            } else {
                ByteInputStream is(mb);
                if (m_cellTree != 0) {
                    delete m_cellTree;
                }
                m_cellTree = new CellTree(is);
                //String treeStr(m_cellTree->toStringPerLevel());
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - tree\n%s\n"), treeStr.c_str()));
                ACE_Message_Block::release(mb);
            }
            if (client->setCloseListener(this) == -1) {
                client->close();
                //delete client;
                m_client.reset(0);
                throw ServiceException("Error connecting to coordinator!");
            }


            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - join ret(%d)\n"), joinRet));
        }
        ACE_Time_Value end = ACE_OS::gettimeofday();
        end -= start;
        TraceRuntimeSingleton::instance()->logMembershipRebindTime(end);
        /*ByteInputStream is(mb);
        if (m_cellTree != 0) {
            delete m_cellTree;
        }
        m_cellTree = new CellTree(is);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:rebindToNewCoordinator() - tree\n%s\n"), m_cellTree->toStringPerLevel().c_str()));
        ACE_Message_Block::release(mb);*/
    } catch (CellException& joinEx) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell:rebindToNewCoordinator() - ex1\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
    return true;

}

void Cell::handleCoordinatorFailure(UUIDPtr& uuid) {
    //if (m_debugCell) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() before lock\n")));
    //}
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    //if (m_debugCell) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure()\n")));
    //}
    UUIDPtr runtimeUUID;
    this->getPID(runtimeUUID);

    CellTreeNode* rootNode = m_cellTree->getRootNode();
    P3PeerInfoPtr pi;
    rootNode->getCoordinator(pi);
    if (!pi.null() && pi->getPID()->isEqual(uuid)) {
        rootNode->remove(uuid);
        //P3PeerInfo* newCoord = rootNode->getCoordinator();
        P3PeerInfoPtr newCoord;
        rootNode->getCoordinator(newCoord);
        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - next coordinator endpoint %s %s\n"),
                    newCoord->getPID()->toString().c_str(), newCoord->getCellEndpoint()->toString().c_str()));
        }
        if (runtimeUUID->isEqual(newCoord->getPID())) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - COORDINATOR\n")));
            m_coordinator = true;
            if (!this->m_parentCellID.null() && (!this->m_parentCellID->isEqual(CellID::INVALID_CELL_ID_UUIDPTR))) {
                /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - coord - rebinding parentCellID(%s)\n"),
                        m_parentCellID->toString().c_str()));*/
                fireRebind();
                if (m_debugCell) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - coord - after rebinding\n")));
                }
            } else {
                if (m_debugCell) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - rooot coord ready\n")));
                }
            }
            list<UUIDPtr> l;
            this->m_childrenGroup->getPeerList(l);
            //ensures that clients that arent connected are removed
            m_cellTree->pruneSubtrees(l);
            //}
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - rebind to new coord\n")));
            //follower connect to coordinator
            rebindToNewCoordinator((UUIDPtr&) newCoord->getPID(), newCoord->getCellEndpoint());
        }

    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleCoordinatorFailure() - wrong coordinator %s %s?!!!!!!\n"),
                pi->getPID()->toString().c_str(), uuid->toString().c_str()));
    }
    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:handleCoordinatorFailure() - tree:\n%s\n"),
            m_cellTree->toString().c_str()));*/
}

void Cell::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::onClose before lock()\n")));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::onClose()\n")));
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);


    channel->setCloseListener(0);

    if (!m_client.null() && m_client.get() == channel) {
        //follower        
        //TODO:
        //CellClientHandler* clientHandler = static_cast<CellClientHandler*> (channel);
        //handleCoordinatorFailure(clientHandler->getPID());
        UUIDPtr uuid = channel->getPID();
        m_client.reset(0);
        handleCoordinatorFailure(uuid);
        return;
    } else {
        //coordinator
        if (m_parentChannel.get() == channel) {
            TraceRuntimeSingleton::instance()->logUnbind(runtimeUUID);
            //parent bind lost, must rebind
            m_parentChannel.reset(0);
            m_parentUUID.reset(0);
            m_parentCellID.reset(0);
            m_parentDiscoverySAP.reset(0);
            m_parentMeshSAP.reset(0);
            m_parentFTSAP.reset(0);
            fireRebind();
            return;
        } else {
            CellSvcHandler* svcHandler = static_cast<CellSvcHandler*> (channel);
            UUIDPtr uuid = channel->getPID();
            CellIDPtr cellID = svcHandler->getCellID();
            leaveMesh(uuid, cellID);
            return;
        }
    }

}

//TODO: remvoe this

void Cell::onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {

}

int Cell::handleCoordinatorRebindMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, RebindMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRebindMesh() before lock- %@\n"), packet->getCellID().get()));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    UUIDPtr cellID(m_cellID->duplicate());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRebindMesh() - %@\n"), packet->getCellID().get()));
    }

    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);


    UUIDPtr fid(packet->getCellID()->duplicate());
    SAPInfoPtr discoverySAP = packet->getDiscoverySAP();
    SAPInfoPtr meshSAP = packet->getMeshSAP();
    SAPInfoPtr ftSAP = packet->getFTSAP();

    P3PeerInfo* pi = new P3PeerInfo(packet->getType(),
            packet->getUUID(),
            fid,
            packet->getEndpoint(),
            meshSAP, discoverySAP, ftSAP);

    int ret = 0;
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRebindMesh() - %s %s\n"), packet->getCellID()->toString().c_str(),
                m_cellID->toString().c_str()));
    }
    if (runtimeCellID->isEqual(*(fid.get()))) {
        ret = m_cellTree->addPeer(pi);
    } else {
        ret = m_cellTree->addPeer(pi, packet->getParentCellID());
    }
    //hack
    ret = 0;

    bool accepted = false;
    if (ret == 0) {
        //String treeStr(m_cellTree->toStringPerLevel());
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorRebindMesh() - tree\n%s\n"), treeStr.c_str()));
        accepted = true;
    }


    SthenoPacket* joinReply =
            new RebindMeshReplyPacket(
            runtimeUUID,
            cellID,
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            accepted);
    ACE_Time_Value* timeout = 0;
    channel->sendPacket(joinReply, timeout);
    delete packet;
    delete joinReply;
    return 0;
}

/*Follower*/
int Cell::handleCoordinatorJoinMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, JoinMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorJoinMesh() before lock- %@\n"), packet->getCellID().get()));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);

    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);


    UUIDPtr cellID(m_cellID->duplicate());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorJoinMesh() - %@\n"), packet->getCellID().get()));
    }
    UUIDPtr fid(packet->getCellID()->duplicate());
    SAPInfoPtr discoverySAP = packet->getDiscoverySAP();
    SAPInfoPtr meshSAP = packet->getMeshSAP();
    SAPInfoPtr ftSAP = packet->getFTSAP();


    P3PeerInfo* pi = new P3PeerInfo(packet->getType(),
            packet->getUUID(),
            fid,
            packet->getEndpoint(),
            meshSAP, discoverySAP, ftSAP);

    int ret = 0;
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorJoinMesh() - %s %s\n"), packet->getCellID()->toString().c_str(),
                m_cellID->toString().c_str()));
    }
    if (runtimeCellID->isEqual(*(fid.get()))) {
        ret = m_cellTree->addPeer(pi);
    } else {
        ret = m_cellTree->addPeer(pi, packet->getParentCellID());
    }
    ret = 0;

    bool accepted = false;
    if (ret == 0) {
        // String treeStr(m_cellTree->toStringPerLevel());
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorJoinMesh() - tree\n%s\n"), treeStr.c_str()));
        accepted = true;
    }


    SthenoPacket* joinReply =
            new JoinMeshReplyPacket(
            runtimeUUID,
            cellID,
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            accepted);
    ACE_Time_Value* timeout = 0;
    channel->sendPacket(joinReply, timeout);
    delete packet;
    delete joinReply;
    return 0;
}

int Cell::handleCoordinatorLeaveMesh(/*CellClientHandler*/CellFollowerSvcHandler* channel, LeaveMeshPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() before lock\n")));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh()\n")));
    }
    Boolean status = true;
    CellTreeNode* node = m_cellTree->getCellTreeNode(packet->getCellID());
    if (node == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() - didnt find fid!\n")));
        status = false;
    } else {
        if (node->remove(packet->getUUID()) != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() - found fid but didnt manage to delete\n")));
            status = false;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() - found fid and delete!\n")));
            if (node->getPeerCount() == 0) {
                m_cellTree->removeSubtree(packet->getCellID());
            }
        }
    }
    //String treeStr(m_cellTree->toStringPerLevel());
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() - tree\n%s\n"), treeStr.c_str()));

    UUIDPtr cellID(new CellID(runtimeCellID.get()));
    SthenoPacket* reply = new LeaveMeshReplyPacket(
            runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            status);
    ACE_Time_Value* timeout = 0;
    int ret = channel->sendPacket(reply, timeout);
    delete packet;
    delete reply;
    return ret;
}

bool Cell::rebindToParentCoordinator(UUIDPtr& parentUUID,
        CellAddrPtr& parentCellAddr,
        EndpointPtr& parentCoordinator) throw (ServiceException&) {
    ACE_Connector<ParentCellClientHandler, ACE_SOCK_Connector> connector;
    m_parentCellID = parentCellAddr->getCellID();

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new ExecutionModelPtr(tpc);

    //ParentCellClientHandler* parentChannel = new ParentCellClientHandler(parentUUID, parentCellAddr->getCellID(), this, true, false, 0, 0, 0, this);
    ParentCellClientHandler* parentChannel = new ParentCellClientHandler(
            parentUUID,
            parentCellAddr->getCellID(),
            this,
            false,
            false,
            tpcPrt,
            0, 0, 0);

    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    (tpc)->bind(parentChannel);
    (tpc)->open(reserve, cpuQoS);

    connector.reactor(tpc->getResources()->getReactor());

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::rebindToParentCoordinator() - Connecting to parent coordinator...(%s,%s)!\n"),
                parentUUID->toString().c_str(),
                parentCoordinator->toString().c_str()));
    }
    if (connector.connect(parentChannel, parentCoordinator->getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: Cell:join() - connect failed:")));
        parentChannel->close();
        delete parentChannel;
        m_parentChannel.reset(0);
        throw ServiceException("Failed to connect to parent coordinator!");
    } else {
        m_parentChannel.reset(parentChannel);
        if (m_parentChannel->asynchronous(true, false) == -1) {
            parentChannel->close();
            //delete parentChannel; //this is controled by the smart pointer!            
            m_parentChannel.reset(0);
            return false;
        }

        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::rebindToParentCoordinator() - Connect OK!\n")));
        }
        //ACE_Message_Block* mb = 0;
        bool joinRet = m_parentChannel->rebindMesh();
        if (!joinRet || m_parentChannel->setCloseListener(this) == -1) {
            parentChannel->close();
            //delete parentChannel; //this is controled by the smart pointer!
            m_parentChannel.reset(0);
            return false;
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::rebindToParentCoordinator() - join ret(%d)\n"), joinRet));
        return true;
    }
}

bool Cell::rebind(UUIDPtr& parentUUID,
        CellAddrPtr& parentCellAddr,
        EndpointPtr& parentCoordinator) {
    if (!m_parentChannel.null()) {
        m_parentCellID = CellID::INVALID_CELL_ID;
        m_parentChannel->close();
        //m_parentChannel = 0;
        m_parentChannel.reset(0);
    }
    if (!parentCoordinator.null()) {
        try {
            return rebindToParentCoordinator(parentUUID,
                    parentCellAddr,
                    parentCoordinator);
        } catch (ServiceException& ex) {
            return false;
        }
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::rebind() - no parent endpoint info?\n")));
    }
    return false;
}

void Cell::sendNotify() {
    if (m_discovery != 0) {
        m_discovery->sendGroupKeepAlive();
    }
    if (m_rootDiscovery != 0) {
        m_rootDiscovery->sendGroupKeepAlive();
    }
}

void Cell::fireRebind() {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    m_rebinding = true;
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);

    //TODO: change this
    QoSManagerInterface* qosManager = getQoSManager();
    CellDiscovery* discovery = new CellDiscovery(runtimeUUID, qosManager);
    discovery->open();
    UUIDPtr cellID(m_cellID->duplicate());
    CellReply* info = discovery->requestCell(cellID);
    while (info != 0) {
        if (m_debugCell) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::firebind() - (%s,%s)\n"),
                    info->getParentUUID()->toString().c_str(), info->getParentCell()->getCellID()->toString().c_str()));
        }

        if (rebind(info->getParentUUID(), info->getParentCell(), info->getParentCoordinatorAddr())) {
            delete discovery;
            delete info;
            m_rebinding = false;
            if (m_notify == 0) {
                m_notify = new NotifyThread(this);
                m_notify->activate();
            }
            ACE_Time_Value end = ACE_OS::gettimeofday();
            end -= start;
            TraceRuntimeSingleton::instance()->logMembershipRebindTime(end);
            return;
        }
        delete info;
        info = discovery->requestCell(cellID);
    }

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::firebind() - NEW ROOT NODE\n")));
    m_parentCellID = CellID::INVALID_CELL_ID;
    if (!m_parentChannel.null()) {
        m_parentChannel->close();
        m_parentChannel.reset(0);
    }
    m_rebinding = false;
    m_rootDiscovery = discovery;
    m_rootDiscovery->attach(this);
    if (m_notify == 0) {
        m_notify = new NotifyThread(this);
        m_notify->activate();
    }

}

void Cell::getNeighbors(list<PeerLocation>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    if (isStarted()) {
        if (m_cellTree != 0) {
            CellTreeNode* node = m_cellTree->getRootNode();
            if (!m_parentChannel.null()) {
                UUIDPtr fid(new UUID(*(m_parentChannel->getCellID().get())));
                PeerLocation loc(m_parentChannel->getPID(), fid);
                l.push_back(loc);
            }
            node->getPeers(l);
            return;
        }
    }
}

void Cell::getDiscoverySAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    UUIDPtr listenerUUID;
    this->m_listener->getUUID(listenerUUID);
    if (uuid->isEqual(listenerUUID)) {
        this->m_listener->getOverlay_i()->getP3Discovery()->getSAP(si);
        if (m_debugCell) {
            printf("Cell::getDiscoverySAPInfo - OUR %p\n", si.get());
        }
        return;
    }
    UUIDPtr parentUUID;
    m_listener->getParentUUID(parentUUID);
    if (uuid->isEqual(parentUUID)) {
        si = m_parentDiscoverySAP;
        if (m_debugCell) {
            printf("Cell::getDiscoverySAPInfo - parent %p\n", si.get());
        }
        return;
    }
    //if (isStarted()) {
    P3PeerInfoPtr peerInfo;
    if (this->getPeer(uuid, peerInfo) == -1) {
        printf("Didnt find peer\n");
        throw ServiceException("Invalid Peer");
    }

    printf("Cell::getDiscoverySAPInfo - getDiscoverySAPInfo - %p\n", peerInfo.get());
    /*if(peerInfo.null()){
        printf("Cell::getDiscoverySAPInfo - getDiscoverySAPInfo - null");
    }else{
        printf("Cell::getDiscoverySAPInfo - getDiscoverySAPInfo %s\n", peerInfo->getPID()->toString().c_str());
    }*/
    si = peerInfo->getDiscoverySAP();

    return;
    //}
}

void Cell::getMeshSAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    UUIDPtr listenerUUID;
    this->m_listener->getUUID(listenerUUID);
    if (uuid->isEqual(listenerUUID)) {
        this->m_listener->getOverlay_i()->getP3MeshService()->getSAP(si);
        if (m_debugCell) {
            printf("Cell::getMeshSAPInfo - OUR %p\n", si.get());
        }
        return;
    }
    UUIDPtr parentUUID;
    m_listener->getParentUUID(parentUUID);
    if (uuid->isEqual(parentUUID)) {
        si = m_parentMeshSAP;
        if (m_debugCell) {
            printf("Cell::getMeshSAPInfo - parent %p\n", si.get());
        }
        return;
    }

    //if (isStarted()) {
    P3PeerInfoPtr peerInfo;
    if (this->getPeer(uuid, peerInfo) == -1) {
        throw ServiceException("Invalid Peer");
    }
    si = peerInfo->getMeshSAP();
    return;
    //}
}

void Cell::getFTSAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    UUIDPtr listenerUUID;
    this->m_listener->getUUID(listenerUUID);
    if (uuid->isEqual(listenerUUID)) {
        FaultTolerancePtr ftPtr;
        try {
            this->m_listener->getOverlay_i()->getFaultTolerance(ftPtr);
        } catch (OverlayException& ex) {
            throw ServiceException(ServiceException::LOW_LVL_P2P_UNAVAILABLE);
        }
        ftPtr->getSAP(si);
        if (m_debugCell) {
            printf("Cell::getDiscoverySAPInfo - OUR %p\n", si.get());
        }
        return;
    }

    if (uuid->isEqual(listenerUUID)) {
        si = m_parentFTSAP;
        if (m_debugCell) {
            printf("Cell::getDiscoverySAPInfo - parent %p\n", si.get());
        }
        return;
    }
    if (m_debugCell) {
        printf("getDiscoverySAPInfo: ******************************** %s %s ************************\n",
                listenerUUID->toString().c_str(),
                uuid->toString().c_str()
                );
    }
    //if (isStarted()) {
    P3PeerInfoPtr peerInfo;
    if (this->getPeer(uuid, peerInfo) == -1) {
        throw ServiceException("Invalid Peer");
    }
    si = peerInfo->getFTSAP();

    return;
    //}
}

int Cell::processUpdateInfo(InfoUpdatePtr& updateInfo) {
    switch (updateInfo->getType()) {
        case MeshUpdateInfo::MESH_UPDATEINFO_TYPE:
        {
            switch (updateInfo->getOperation()) {
                case AddServiceUpdateInfo::ADD_SERVICE_UPDATEINFO:
                {
                    if (m_debugCell) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO\n")));
                    }
                    AddServiceUpdateInfo* addInfo = static_cast<AddServiceUpdateInfo*> (updateInfo.get());
                    if (m_debugCell) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - uuid(%s) id(%s)\n"),
                                addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                    }
                    CellTreeNode* node = m_cellTree->getCellTreeNode(addInfo->getFID());
                    if (node == 0) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - FAILED uuid(%s) id(%s)\n"),
                                addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                        break;
                    }
                    P3PeerInfoPtr peerInfo;
                    if (node->getPeer(addInfo->getPID(), peerInfo) != 0) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - Peer not known! uuid(%s) id(%s)\n"),
                                addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                        break;
                    }
                    /*list<EndpointPtr> l;
                    addInfo->getServiceInfo()->getEndpointList(l);
                    if (!peerInfo->addServiceInstance(addInfo->getServiceInfo()->getSID(), addInfo->getServiceInfo()->getIID(), l)) {
                        break;
                    }*/
                    if (m_debugCell) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - adding to peer - uuid(%s) id(%s)\n"),
                                addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - adding to peer - %@\n"),
                                peerInfo.get()));
                    }

                    SAPInfoPtr sapInfo;
                    sapInfo = addInfo->getServiceInfo()->getSAPInfo();
                    if (!peerInfo->addServiceInstance(addInfo->getServiceInfo()->getSID(), addInfo->getServiceInfo()->getIID(), sapInfo)) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - failed to add instance - uuid(%s) id(%s)\n"),
                                addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                        break;
                    }
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - ADD_SERVICE_UPDATEINFO - uuid(%s) id(%s) OK!\n"),
                            addInfo->getPID()->toString().c_str(), addInfo->getServiceInfo()->getIID()->toString().c_str()));
                    return 0;
                }
                case RemoveServiceUpdateInfo::REMOVE_SERVICE_UPDATEINFO:
                {
                    RemoveServiceUpdateInfo* removeInfo = static_cast<RemoveServiceUpdateInfo*> (updateInfo.get());
                    CellTreeNode* node = m_cellTree->getCellTreeNode(removeInfo->getFID());
                    if (node == 0) {
                        break;
                    }
                    P3PeerInfoPtr peerInfo;
                    if (node->getPeer(removeInfo->getPID(), peerInfo) != 0) {
                        break;
                    }
                    if (!peerInfo->removeService(removeInfo->getSID(), removeInfo->getIID())) {

                        break;
                    }
                    return 0;
                }

                case ChangegIIDServiceUpdateInfo::UPDATE_IID_SERVICE_UPDATEINFO:
                {
                    ChangegIIDServiceUpdateInfo* update = static_cast<ChangegIIDServiceUpdateInfo*> (updateInfo.get());
                    CellTreeNode* node = m_cellTree->getCellTreeNode(update->getFID());
                    if (node == 0) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - UPDATE_IID_SERVICE_UPDATEINFO - FAILED uuid(%s)\n"),
                                update->getPID()->toString().c_str()));
                        break;
                    }
                    P3PeerInfoPtr peerInfo;
                    if (node->getPeer(update->getPID(), peerInfo) != 0) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - UPDATE_IID_SERVICE_UPDATEINFO - Peer not known! uuid(%s)\n"),
                                update->getPID()->toString().c_str()));
                        break;
                    }
                    bool ret = peerInfo->changeServiceIID(update->getSID(), update->getOldIID(), update->getNewIID());
                    ACE_DEBUG((LM_DEBUG,
                            ACE_TEXT("(%t|%T)INFO: Cell::processUpdateInfo() - UPDATE_IID_SERVICE_UPDATEINFO - Upate OK from (%s) to(%s)\n"),
                            update->getOldIID()->toString().c_str(), update->getNewIID()->toString().c_str()));
                    return 0;
                }

                default:
                    break;

            }
        }
        case SensorInfoUpdate::SENSOR_UPDATE_TYPE:
        {
            SensorInfoUpdate* sensorUpdate = static_cast<SensorInfoUpdate*> (updateInfo.get());
            list<SensorInfoPtr>* l = 0;
            int type = 0;
            switch (sensorUpdate->getOperation()) {
                case SensorInfoUpdate::SENSOR_UPDATE_AUDIO_OP:
                {
                    printf("Adding an audio sensor\n");
                    l = &m_audioSensors;
                    type = SensorInfoUpdate::SENSOR_UPDATE_AUDIO_OP;
                    break;
                }
                case SensorInfoUpdate::SENSOR_UPDATE_VIDEO_OP:
                {
                    l = &m_videoSensors;
                    type = SensorInfoUpdate::SENSOR_UPDATE_VIDEO_OP;
                    break;
                }
                case SensorInfoUpdate::SENSOR_UPDATE_EVENT_OP:
                {
                    l = &m_eventSensors;
                    type = SensorInfoUpdate::SENSOR_UPDATE_EVENT_OP;
                    break;
                }
                case SensorInfoUpdate::SENSOR_UPDATE_ACTUATOR_OP:
                {
                    printf("Adding an actuator sensor\n");
                    l = &m_actuatorSensors;
                    type = SensorInfoUpdate::SENSOR_UPDATE_ACTUATOR_OP;
                    break;
                }
            }
            list<SensorInfoPtr>::iterator iter = l->begin();
            bool found = false;
            while (iter != l->end()) {
                if ((*iter)->getUUID()->isEqual(sensorUpdate->getUUID())) {
                    found = true;
                    break;
                }
                iter++;
            }
            if (!found) {
                SensorInfo* su = new SensorInfo(type, sensorUpdate->getUUID(), sensorUpdate->getEndpoint());
                SensorInfoPtr suPtr(su);
                l->push_back(suPtr);
                printf("Added sensor!\n");
            }
            if (m_debugCell) {
                printf("*****************************+SENSOR=%s %s\n", sensorUpdate->getUUID()->toString().c_str(),
                        sensorUpdate->getEndpoint()->toString().c_str());
            }
            return 0;
        }
        default:
            break;
    }
    return -1;
}

void Cell::updateState(InfoUpdate* updateInfo) throw (ServiceException&) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::updateState() before lock\n")));
    }
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::updateState()\n")));
    }
    //free update pointer on stack pop
    InfoUpdatePtr updateInfoPtr(updateInfo);
    processUpdateInfo(updateInfoPtr);
    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    ace_mon.release();

    if (this->IsCoordinator()) {
        //send to parent
        if (!parentHandler.null()) {
            if (!parentHandler->updateInfo(updateInfoPtr)) {

                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::updateState() - failed to update to parent!\n")));
                //must restart bind process and find a suitable parent
            }
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - send reply to group\n")));
        UUIDPtr runtimeUUID;
        this->getPID(runtimeUUID);
        m_cellFollowerGroup->updateInfo(runtimeUUID, updateInfoPtr);
    } else {
        m_client->updateInfo(updateInfoPtr);
    }
}

int Cell::handleUpdateInfo(CellSvcHandler* channel, UpdateInfoPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo() before Lock\n")));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo()\n")));
    }
    Boolean status = true;

    InfoUpdate* updateInfo = InfoUpdateFactory::createInfoUpdate(packet->deattachUpdateInfo());
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo() - afer factory\n")));
    }
    InfoUpdatePtr updateInfoPtr(updateInfo);
    if (processUpdateInfo(updateInfoPtr) != 0) {
        status = false;
    }
    //ace_mon.release();
    ParentCellClientHandlerPtr parentHandler;
    this->getParentHandler(parentHandler);
    //String treeStr(m_cellTree->toStringPerLevel());
    ace_mon.release();

    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo() - afer process\n")));
    }
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);
    UUIDPtr cellID(new CellID(runtimeCellID.get()));
    SthenoPacket* reply = new UpdateInfoReplyPacket(
            runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            status);

    ACE_Time_Value* timeout = 0;
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo() -before parent!\n")));
    }
    if (this->IsCoordinator()) {
        //send to parent
        if (!parentHandler.null()) {
            if (!parentHandler->updateInfo(updateInfoPtr)) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: Cell::handleUpdateInfo() - failed to update to parent!\n")));
                //must restart bind process and find a suitable parent
            }
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - send reply to group\n")));
        m_cellFollowerGroup->updateInfo(packet->getPacketHeader()->getSrcPID(), updateInfoPtr);
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleLeaveMesh() - send reply to child\n")));
    int ret = channel->sendPacket(reply, timeout);

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleUpdateInfo() - tree\n%s\n"), treeStr.c_str()));


    delete packet;
    delete reply;
    return ret;

}

int Cell::handleCoordinatorUpdateInfo(/*CellClientHandler**/CellFollowerSvcHandler* channel, UpdateInfoPacket* packet) {
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh() before lock\n")));
    }
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    if (m_debugCell) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::handleCoordinatorLeaveMesh()\n")));
    }
    Boolean status = true;

    InfoUpdate* updateInfo = InfoUpdateFactory::createInfoUpdate(packet->deattachUpdateInfo());
    InfoUpdatePtr updateInfoPtr(updateInfo);
    if (processUpdateInfo(updateInfoPtr) != 0) {
        status = false;
    }
    ace_mon.release();
    UUIDPtr runtimeUUID;
    getPID(runtimeUUID);
    CellIDPtr runtimeCellID;
    getCellID(runtimeCellID);
    UUIDPtr cellID(new CellID(runtimeCellID.get()));
    SthenoPacket* reply = new UpdateInfoReplyPacket(
            runtimeUUID, cellID, //(UUIDPtr&) getCellID(),
            packet->getPacketHeader()->getSrcPID(),
            packet->getPacketHeader()->getSrcFID(),
            0,
            packet->getPacketHeader()->getPacketID(),
            status);
    ACE_Time_Value* timeout = 0;
    int ret = channel->sendPacket(reply, timeout);
    delete packet;
    return ret;
}

bool Cell::isClosed() {
    return m_closed;
}

bool Cell::isRebinding() {
    return m_rebinding;
}

bool Cell::isRoot() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), false);
    return (m_parentCellID.null() || m_parentCellID->isEqual(CellID::INVALID_CELL_ID_UUIDPTR));
}

bool Cell::isStarted() {
    return m_started;
}

void Cell::getAudioSensors(list<SensorInfoPtr>& audioL) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    list<SensorInfoPtr>::iterator iter = m_audioSensors.begin();
    while (iter != m_audioSensors.end()) {
        audioL.push_back(*iter);
        iter++;
    }
}

void Cell::getVideoSensors(list<SensorInfoPtr>& videoL) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    list<SensorInfoPtr>::iterator iter = m_videoSensors.begin();
    while (iter != m_videoSensors.end()) {
        videoL.push_back(*iter);
        iter++;
    }

}

void Cell::getEventSensors(list<SensorInfoPtr>& eventL) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    list<SensorInfoPtr>::iterator iter = m_eventSensors.begin();
    while (iter != m_eventSensors.end()) {
        eventL.push_back(*iter);
        iter++;
    }
}

void Cell::getActuatorSensors(list<SensorInfoPtr>& actuatorL) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    list<SensorInfoPtr>::iterator iter = m_actuatorSensors.begin();
    while (iter != m_actuatorSensors.end()) {
        actuatorL.push_back(*iter);
        iter++;
    }

}

ACE_SYNCH_RECURSIVE_MUTEX& Cell::getLock() {
    return m_lock;
}

void Cell::getSensors(list<P3PeerInfoPtr>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    this->m_cellTree->getSensors(l);
}

void Cell::getPoL(P3PeerInfoPtr& peer, list<UUIDPtr>& filter) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    this->m_cellTree->getPoL(peer, filter);
}

int Cell::getPeerByIID(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    return this->m_cellTree->getPeerByIID(sid, iid, info);
}

int Cell::getPeer(const UUIDPtr& pid, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(), -1);
    return this->m_cellTree->getPeer(pid, info);
}

void Cell::serializeState(OutputStream& outputStream) throw (SerializationException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    this->m_cellTree->serialize(outputStream);
}


/*int Cell::handleCoordinatorBind(CellFollowerSvcHandler* channel){
    m_coordinatorChannel = channel;
    channel->open();
    channel->asynchronous(true,true);
    return 0;
}*/
