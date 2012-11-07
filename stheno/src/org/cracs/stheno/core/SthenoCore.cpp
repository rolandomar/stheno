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
 * File:   SthenoCore.cpp
 * Author: rmartins
 * 
 * Created on July 27, 2010, 3:02 PM
 */
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>

#include "SthenoCore.h"
#include "LocalService.h"
#include <euryale/qos/medusa/Cgroup.h>

#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/service/UserService.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/SthenoCoreInterface.h>

#include <stheno/core/p2p/discovery/DiscoveryInterface.h>
#include <stheno/core/p2p/OverlayInterface.h>

#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQuery.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQueryReply.h>

#include <stheno/core/p2p/discovery/general_provider/FindPeerQuery.h>
#include <stheno/core/p2p/discovery/general_provider/FindPeerQueryReply.h>
//#include <stheno/services/audio/AudioClient.h>
#include <ace/Signal.h>

//#include <stheno/core/p2p/p3/superpeer/SuperPeer.h>

//hardcoded must be changed

void SthenoCore::createRuntime(UUIDPtr& rid, SthenoCorePtr& core) throw (RuntimeException&) {
    SthenoCore* runtime = new SthenoCore(rid);
    core.reset(runtime);
    //return new SthenoCorePtr(runtime);
}

void SthenoCore::createRuntime(SthenoCorePtr& core) throw (RuntimeException&) {
    SthenoCore* runtime = new SthenoCore();
    core.reset(runtime);
    //return new SthenoCorePtr(runtime);
}

SthenoCore::SthenoCore() :
m_status(NOT_INIT), m_peerID(UUID::generateUUID()), m_overlay(0), m_qosManager(this), m_runtimeQoSManager(0),
m_serviceQosManager(0), m_debugStream(0) {
}

SthenoCore::SthenoCore(UUIDPtr& rid) :
m_status(NOT_INIT), m_peerID(rid), m_overlay(0), m_qosManager(this), m_serviceQosManager(0), m_runtimeQoSManager(0), m_debugStream(0) {
}

SthenoCore::~SthenoCore() {
    if (m_debugStream != 0) {
        delete m_debugStream;
    }
}

bool SthenoCore::trace() {
    return m_trace;
}

Overlay* SthenoCore::getOverlay() throw (RuntimeException&) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    /*if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }*/
    if (m_overlay == 0) {
        //specific implementation of the p3 arch
        //m_overlay = new SuperPeer(this);
    }
    return m_overlay;
}

void SthenoCore::initRuntimeQoS(ACE_ARGV& args) throw (RuntimeException&) {
    if (m_reservationFlag) {
        try {
            m_qosManager.open();
        } catch (CgroupException& ex) {
            throw RuntimeException(RuntimeException::QOS_CLIENT_INITIALIZATION_ERROR);
        }
    }
}

void SthenoCore::initLogging(const char* statsFile, const char* tracefile, const char* debugfile) throw (RuntimeException&) {
    LogStatisticsSingleton::instance()->open(statsFile);
    TraceRuntimeSingleton::instance()->open(tracefile);
    ACE_OSTREAM_TYPE *m_debugStream =
            new std::ofstream(debugfile);
    ACE_LOG_MSG->msg_ostream(m_debugStream, 1);
    ACE_LOG_MSG->set_flags(ACE_Log_Msg::OSTREAM);
    ACE_LOG_MSG->set_flags(ACE_Log_Msg::STDERR);
    //String strtest = "test\n";
    //LogStatisticsSingleton::instance()->logLine(strtest);
}

void SthenoCore::open(ACE_ARGV& args) throw (RuntimeException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);

    m_reservationFlag = false;
    m_runNO = 0; //no run
    m_testNO = 0; //no test
    m_trace = true;
    m_maxServicesQoS = 2;
    //m_runtimeQoS = 950000;
    //absolute
    // dont need this
    m_runtimeQoS = RUNTIME_DEFAULT_QOS; //5000;
    m_runtimePeriod = CPUReservation::DEFAULT_PERIOD;

    //signal handling
    ACE_Sig_Action no_sigpipe((ACE_SignalHandler) SIG_IGN);
    ACE_Sig_Action original_action;
    no_sigpipe.register_action(SIGPIPE, &original_action);


    static const ACE_TCHAR options[] = ACE_TEXT("::");
    const char* prefix = "\0";
    ACE_Get_Opt cmd_opts(args.argc(), args.argv(), options);
    int option;
    if (cmd_opts.long_option
            (ACE_TEXT("prefix"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("run"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("test"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("reservation"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("maxservices"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("runtimeqos"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }

    if (cmd_opts.long_option
            (ACE_TEXT("runtimeqosperiod"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        throw RuntimeException(RuntimeException::INVALID_ARGS);
    }
    printf("AGRS=%d\n", args.argc());

    while ((option = cmd_opts()) != EOF)
        switch (option) {
            case 0:
            {
                //long_option
                const char *op = cmd_opts.long_option();
                printf("LONG=%s\n", cmd_opts.opt_arg());
                if (ACE_OS::strcmp("prefix", op) == 0) {
                    prefix = cmd_opts.opt_arg();
                    printf("Prefix=%s\n", prefix);
                    break;
                }
                if (ACE_OS::strcmp("run", op) == 0) {
                    m_runNO = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("RunNo=%d %s\n", m_runNO, cmd_opts.opt_arg());
                    break;
                }

                if (ACE_OS::strcmp("test", op) == 0) {
                    m_testNO = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("TestNo=%d %s\n", m_testNO, cmd_opts.opt_arg());
                    break;
                }
                if (ACE_OS::strcmp("reservation", op) == 0) {
                    if (strcasecmp(cmd_opts.opt_arg(), "true") == 0) {
                        m_reservationFlag = true;
                    }
                    break;
                }
                if (ACE_OS::strcmp("maxservices", op) == 0) {
                    m_maxServicesQoS = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    break;
                }

                if (ACE_OS::strcmp("runtimeqos", op) == 0) {
                    m_runtimeQoS = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("runtimeqos=%d %s\n", m_runtimeQoS, cmd_opts.opt_arg());
                    break;
                }

                if (ACE_OS::strcmp("runtimeqosperiod", op) == 0) {
                    m_runtimePeriod = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("runtimeqosPeriod=%d %s\n", m_runtimeQoS, cmd_opts.opt_arg());
                    break;
                }
                break;
            }
            case ':':
            {
                ACE_ERROR
                        ((LM_ERROR, ACE_TEXT("-%c requires an argument\n"),
                        cmd_opts.opt_opt()));
                throw RuntimeException(RuntimeException::INVALID_ARGS);
            }
            default:
            {
            }
        }


    m_p2pQoS = P2P_DEFAULT_QOS; //5000;
    m_servicesQoS = SERVICE_MANAGER_DEFAULT_QOS; //5000;
    //total=75K
    //m_perServicesQoS = 25000;//25000;
    /*m_p2pQoS = m_runtimeQoS * 0.2;    
    m_servicesQoS = m_runtimeQoS * 0.8;        
    m_perServicesQoS = m_servicesQoS / m_maxServicesQoS;*/

    //services 100+50 = 150k
    //core 10k
    //p2p 125k
    //

    stringstream statFile;
    stringstream traceFile;
    stringstream debugFile;
    statFile << "stat_" << prefix << "_" << m_testNO << "_" << m_runNO << "_" << this->m_peerID->toString().c_str() << ".log";
    traceFile << "trace_" << prefix << "_" << m_testNO << "_" << m_runNO << "_" << this->m_peerID->toString().c_str() << ".log";
    debugFile << "debug_" << prefix << "_" << m_testNO << "_" << m_runNO << "_" << this->m_peerID->toString().c_str() << ".log";
    initLogging(statFile.str().c_str(), traceFile.str().c_str(), debugFile.str().c_str());
    initRuntimeQoS(args);
    UUIDPtr runtimeUUID;
    getUUID(runtimeUUID);
    String runtimeDomainPath = "";
    m_runtimeQoSManager = this->getQoSManagerInterface(runtimeDomainPath, RUNTIME_DEFAULT_QOS, m_runtimePeriod, false);
    /*if(m_signalHandler.open()!=0){
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }*/

    try {
        QoSManagerInterface* qosManager = 0;
        if (m_reservationFlag) {
            //String p2pPath = "p2p";
            //qosManager = this->getQoSManagerInterface(p2pPath, m_p2pQoS, m_runtimePeriod);
            qosManager = m_runtimeQoSManager;
        }
        getOverlay()->open(qosManager);
        if (m_reservationFlag) {
            //String servicesPath = "services";
            //m_serviceQosManager = this->getQoSManagerInterface(servicesPath, m_servicesQoS, m_runtimePeriod);            
            m_serviceQosManager = m_runtimeQoSManager;
        }

    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    } catch (RuntimeException& runtimeEx) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    toStart();
    if (trace()) {
        TraceRuntimeSingleton::instance()->logRuntimeOpen(this->m_peerID);
    }
}

void SthenoCore::close() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toClose();
    ACE_Hash_Map_Manager<UUIDPtr, LocalServicePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_serviceMap.begin();
    while (iter != m_serviceMap.end()) {
        if (!iter->item().null()) {
            iter->item()->close();
        }
        iter++;
    }
    m_serviceMap.close();
    if (m_overlay != 0) {
        m_overlay->close();
    }
    m_qosManager.close();
    TraceRuntimeSingleton::instance()->logRuntimeClose(this->m_peerID);
}

const UUIDPtr& SthenoCore::getSID(const char* name) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    return m_serviceFactory.getServiceName(name);
}

void SthenoCore::changeIIDOfService(UUIDPtr& sid, UUIDPtr& iid, UUIDPtr& newIid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }

    servicePtr->setIID(iid, newIid);
    ServiceInstanceInfoPtr info;
    servicePtr->getInstancesOfServiceInfo(newIid, info);
    //info has the newIid
    MeshPtr meshPtr;
    try {
        getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    meshPtr->onServiceUpdateIID(sid, iid, newIid);
}

void SthenoCore::startService(const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid, UUIDPtr& deployUUIDD) throw (RuntimeException&, ServiceException&) {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    list<UUIDPtr> filterList;
    UUIDPtr runtimeUUID;
    this->getUUID(runtimeUUID);
    LaunchServiceSiteQuery* query = new LaunchServiceSiteQuery(runtimeUUID, sid, filterList);
    DiscoveryPtr discovery;
    try {
        m_overlay->getDiscovery(discovery);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    //DiscoveryQueryReply* reply = m_overlay->getDiscovery()->executeQuery(query);
    DiscoveryQueryReply* reply = discovery->executeQuery(query);
    if (reply == 0) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    LaunchServiceSiteQueryReply* replyNarrow = new LaunchServiceSiteQueryReply(reply);
    //printf("PoL UUID=%s\n", replyNarrow->getUUID()->toString().c_str());
    deployUUIDD = replyNarrow->getUUID();
    //printf("Before service remote deployment\n");
    MeshPtr meshPtr;
    try {
        getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    //getOverlay()->getMesh()->createRemoteService(replyNarrow->getMeshSAPInfo().get(), replyNarrow->getUUID(), sid, params, iid);
    try {
        meshPtr->createRemoteService(replyNarrow->getMeshSAPInfo().get(), replyNarrow->getUUID(), sid, params, iid);
    } catch (ServiceException& se) {
        delete replyNarrow;
        throw se;
    }
    //printf("After service remote deployment\n");
    ACE_Time_Value end = ACE_OS::gettimeofday();
    end -= start;
    if (trace()) {
        MeshPtr meshPtr;
        this->getOverlay()->getMesh(meshPtr);
        UUIDPtr fid;
        meshPtr->getFID(fid);
        if (params->getFTParams().null()) {
            TraceRuntimeSingleton::instance()->logRemoteServiceCreation(this->m_peerID, fid, sid, iid, end);
        } else {
            TraceRuntimeSingleton::instance()->logRemoteServiceCreationFT(this->m_peerID, fid, sid, iid, end);
        }
    }
    delete replyNarrow;
}

void SthenoCore::startService(UUIDPtr& sid, UUIDPtr& peerID, ServiceParamsPtr& params, UUIDPtr& iid) throw (RuntimeException&, ServiceException&) {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    FindPeerQuery* queryPeer = new FindPeerQuery(peerID);
    DiscoveryPtr discovery;
    try {
        m_overlay->getDiscovery(discovery);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    DiscoveryQueryReply* replyPeer = discovery->executeQuery(queryPeer);
    FindPeerQueryReply* replyPeerNarrow = new FindPeerQueryReply(replyPeer);
    if (replyPeerNarrow->isException()) {
        printf("Peer is now known!\n");
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    printf("Before service remote deployment SAP=%s\n", replyPeerNarrow->getPeerInfo()->getFID()->toString().c_str());
    MeshPtr meshPtr;
    try {
        getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    try {
        meshPtr->createRemoteService(replyPeerNarrow->getPeerInfo()->getMeshSAP().get(), peerID, sid, params, iid);
    } catch (ServiceException& se) {
        delete replyPeerNarrow;
        throw se;
    }
    printf("After service remote deployment\n");
    ACE_Time_Value end = ACE_OS::gettimeofday();
    end -= start;
    if (trace()) {
        MeshPtr meshPtr;
        this->getOverlay()->getMesh(meshPtr);
        UUIDPtr fid;
        meshPtr->getFID(fid);
        if (params->getFTParams().null()) {
            TraceRuntimeSingleton::instance()->logRemoteServiceCreation(this->m_peerID, fid, sid, iid, end);
        } else {
            TraceRuntimeSingleton::instance()->logRemoteServiceCreationFT(this->m_peerID, fid, sid, iid, end);
        }
    }
    delete replyPeerNarrow;
}

void SthenoCore::stopLocalService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    removeService(sid, iid);
}

void SthenoCore::stopService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    //find peerID that has (sid,iid)
    //UUIDPtr& peerID = getOverlay()->getDiscovery()->locateService(sid,iid,rtParams);
    //if local
    stopLocalService(sid, iid);
    //else
    //getOverlay()->getMesh()->closeRemoteService(peerID,sid);

    throw RuntimeException("Not implemented!");
}

void SthenoCore::getUUID(UUIDPtr& uuid) {
    uuid = m_peerID;
}

void SthenoCore::onServiceClose(UserService* service) {
    throw RuntimeException("Not implemented!");
}

ServiceClient* SthenoCore::getClient(const UUIDPtr& sid, const UUIDPtr& iid, ClientParamsPtr& clientParams)
throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    printf("SthenoCore::getClient 1\n");
    ServiceAbstract* sa = m_serviceFactory.getInstance(sid);
    printf("SthenoCore::getClient 2\n");
    UserService* us = static_cast<UserService*> (sa);
    printf("SthenoCore::getClient 3\n");
    ServiceClient* client = us->getClient(sid, iid, clientParams);
    printf("SthenoCore::getClient 4\n");
    delete sa;
    printf("SthenoCore::getClient 5 %p\n", client);
    return client;
}

ServiceClient* SthenoCore::getClient(const UUIDPtr& sid, ClientParamsPtr& clientParams) throw (RuntimeException&, ServiceException&) {
    throw RuntimeException("Not implemented!");
}

void SthenoCore::attachOverlay(Overlay* overlay) throw (RuntimeException&) {
    if (m_status != NOT_INIT) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    m_overlay = overlay;
    m_overlay->setRuntime(new SthenoCoreInterface(this));
}

ServiceFactory& SthenoCore::getServiceFactory() {
    return m_serviceFactory;
}

bool SthenoCore::isValid() {
    return (m_status == RUNNING);
}

bool SthenoCore::isSuspended() {
    return (m_status == SUSPENDED);
}

bool SthenoCore::isClose() {
    return (m_status == CLOSED);
}

Byte SthenoCore::getStatus() {
    return m_status;
}

void SthenoCore::toStart() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_status = RUNNING;
}

void SthenoCore::toSuspend() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_status = SUSPENDED;
}

void SthenoCore::toClose() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_status = CLOSED;
}

bool SthenoCore::isServiceRunning(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr localService;
    if (m_serviceMap.find(sid, localService) == -1) {
        return false;
    }
    return localService->existsRunningService(iid);
}

bool SthenoCore::getInstanceOfService(UUIDPtr& sid, UUIDPtr& iid, ServiceAbstractPtr& instance)
throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;

    if (m_serviceMap.find(sid, servicePtr) == -1) {
        throw ServiceException("Service not found!");
    }
    return servicePtr->getInstanceOfService(iid, instance);
}

list<UUIDPtr>* SthenoCore::getInstancesOfServiceUUIDs(UUIDPtr& sid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)S=%s\n"), sid->toString().c_str()));
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        throw ServiceException("Service not found!");
    }
    return servicePtr->getInstancesOfService();
}

void SthenoCore::getInstanceOfService(UUIDPtr& sid, UUIDPtr& iid, ServiceInstanceInfoPtr& info) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::getInstanceOfService(sid,iid,info) - SID=%s IID=%s\n"),
            sid->toString().c_str(),
            iid->toString().c_str()
            ));
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SthenoCore::getInstanceOfService(sid,iid,info) - Error not found SID=%s\n"), sid->toString().c_str()));
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)after ifo S=%s\n"), sid->toString().c_str()));
    servicePtr->getInstancesOfServiceInfo(iid, info);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::getInstanceOfService(sid,iid,info) - Found SID=%s!!!\n"), sid->toString().c_str()));
}

list<ServiceInstanceInfoPtr>* SthenoCore::getInstancesOfService(UUIDPtr& sid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::getInstanceOfService(sid) - IID=%s\n"), sid->toString().c_str()));
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: SthenoCore::getInstanceOfService(sid) - Error not found SID=%s\n"), sid->toString().c_str()));
        throw ServiceException("Service not found!");
    }
    return servicePtr->getInstancesOfServiceInfo();
}

bool SthenoCore::addService(ServiceAbstractPtr& service) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr localService;
    UUIDPtr sid;
    service->getSID(sid);
    if (m_serviceMap.find(sid, localService) == -1) {
        //UUIDPtr localSID;
        //service->getSID(localSID);
        LocalService* li = new LocalService(sid);
        localService.reset(li);
        if (m_serviceMap.bind(sid, localService) == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::addService - failed to bind service!\n")));
            return false;
        }
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::addService - addServiceIsntance...\n")));
    return localService->addServiceInstance(service);
}

void SthenoCore::removeService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr localService;
    if (m_serviceMap.find(sid, localService) == -1) {
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    ServiceAbstractPtr sPtr;
    try {
        localService->removeServiceInstance(iid);
    } catch (ServiceException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SthenoCore:removeService: error on getInstance!\n")));
        throw ex;
    }
}

void SthenoCore::removeService(UUIDPtr& sid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr localService;
    if (m_serviceMap.find(sid, localService) == -1) {
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    if (m_serviceMap.unbind(sid) == 0) {
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
}

void SthenoCore::createLocalService(UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    ServiceAbstractPtr sPtr;
    FaultTolerancePtr ftToleranceSvc;
    try {
        m_overlay->getFaultTolerance(ftToleranceSvc);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    UInt serviceOverallQoS = SERVICE_DEFAULT_QOS * 2; //5000;
    //UInt serviceFTQoS = 50000; //50000;
    //UInt serviceQoS = 50000; //50000;
    //110k
    //total = 31K
    //+1 =46K

    //QoSResources* qos = sPtr->calculateQoSResources(params);    
    //allocate the resources for the primary on the local node, this is done in FT
    //allocateQoS(qos);
    //FT binds the replica information to the instantied service
    //FT allocates the remote QOS resources need to maintain replicas
    printf("==========================> SID=%s FT params=%p %p\n", sid->toString().c_str(), params->getFTParams().get(), params->getQuerySpecializationBlob());
    if (!params->getFTParams().null()) {
        UUIDPtr rgid(UUID::generateUUID());
        iid = rgid;
        createServiceInstance(sid, iid, sPtr);
        //QoS        
        UUIDPtr serviceCgroupUUID(UUID::generateUUID());
        String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();
        QoSManagerInterface* serviceQoSManager = 0;
        if (m_reservationFlag) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::createLocalService() - createSDQM(%s)!\n"), path.c_str()));
            //serviceQoSManager = m_serviceQosManager->createSubDomainQoSManager(path, serviceOverallQoS, m_runtimePeriod);
            m_serviceQosManager = m_runtimeQoSManager;
        }

        QoSManagerInterface* qosFTManager = serviceQoSManager;
        /*String ftPath = Cgroup::getSep();
        ftPath += "FT";
        //HierarchicalPath::appendPaths(ftPath, "/FT");        
        if (m_reservationFlag) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::createLocalService() - FT createSDQM(%s)!\n"), ftPath.c_str()));
            qosFTManager = serviceQoSManager->createSubDomainQoSManager(ftPath, serviceFTQoS, m_runtimePeriod);
        }*/
        QoSManagerInterface* qosServiceManager = serviceQoSManager;
        /*String servicePath = Cgroup::getSep();
        servicePath += "service";
        //HierarchicalPath::appendPaths(servicePath, "/service");        
        if (m_reservationFlag) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SthenoCore::createLocalService() - service createSDQM(%s)!\n"), servicePath.c_str()));
            qosServiceManager = serviceQoSManager->createSubDomainQoSManager(servicePath, serviceQoS, m_runtimePeriod);
        }*/
        //create replication group
        ftToleranceSvc->createReplicationGroup(sPtr, params, rgid, qosFTManager);
        //open service with the proper qos manager
        sPtr->open(params, ServiceAbstract::FT_PRIMARY, qosServiceManager);

        if (trace()) {
            try {
                MeshPtr meshPtr;
                this->getOverlay()->getMesh(meshPtr);
                ReplicationGroupPtr rg;
                ftToleranceSvc->getReplicationGroup(rgid, rg);
                UUIDPtr fid;
                meshPtr->getFID(fid);
                TraceRuntimeSingleton::instance()->logLocalServiceCreationFT(this->m_peerID, fid, sid, iid, rg);
            } catch (OverlayException& oe) {
            }
        }

        //iid = sPtr->getIID();
        /*LocalServicePtr localService;
        if (m_serviceMap.find(sid, localService) == 0) {
            localService->setIID(oldIid, iid);
        }*/
    } else {
        iid.reset(UUID::generateUUID());
        createServiceInstance(sid, iid, sPtr);
        printf("==========================> SID=%s before open service!\n", sid->toString().c_str());
        //UInt serviceOverallQoS = m_perServicesQoS;
        UInt serviceOverallQoS = SERVICE_DEFAULT_QOS; //50000;
        UUIDPtr serviceCgroupUUID(UUID::generateUUID());
        String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();
        QoSManagerInterface* serviceQoSManager = 0;
        if (m_reservationFlag) {
            //serviceQoSManager = m_serviceQosManager->createSubDomainQoSManager(path, serviceOverallQoS, m_runtimePeriod);
            serviceQoSManager = m_runtimeQoSManager;
        }

        sPtr->open(params, ServiceAbstract::FT_NOT_SETTED, serviceQoSManager);
        //iid = sPtr->getIID()
        if (trace()) {
            try {
                MeshPtr meshPtr;
                this->getOverlay()->getMesh(meshPtr);
                UUIDPtr fid;
                meshPtr->getFID(fid);
                TraceRuntimeSingleton::instance()->logLocalServiceCreation(this->m_peerID, fid, sid, iid);
            } catch (OverlayException& oe) {
            }
        }

        printf("==========================> SID=%s after open service!\n", sid->toString().c_str());

    }
    //This is alerady done in createServiceInstance()...
    //start the service finally!    
    /*if (!addService(sPtr)) {
        printf("==========================> faiel to added service!\n");
        //cancel all qos
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }*/

    //inform mesh that a local service has been deployed
    //this can be used to propagate a change in the mesh (cache)
    ServiceInstanceInfoPtr info;
    this->getInstanceOfService(sid, iid, info);
    MeshPtr meshPtr;
    try {
        getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    meshPtr->onServiceCreation(info);
}

void SthenoCore::createLocalServiceReplica(
        UUIDPtr& sid,
        ServiceParamsPtr& params,
        ServiceAbstractPtr& sPtr) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    FaultTolerancePtr ftToleranceSvc;
    try {
        m_overlay->getFaultTolerance(ftToleranceSvc);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    UUIDPtr iid(UUID::generateUUID());
    createServiceInstance(sid, iid, sPtr);
    //QoSResources* qos = sPtr->calculateQoSResources(params);    

    printf("INFO: createLocalServiceReplica() - SID=%s before open service!\n", sid->toString().c_str());
    QoSManagerInterface* qosManager = 0;
    if (m_reservationFlag) {
        UUIDPtr serviceCgroupUUID(UUID::generateUUID());
        String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();

        qosManager = m_serviceQosManager->createSubDomainQoSManager(path, 200, m_runtimePeriod);
        //String ftPath = "/FT";
        String ftPath = Cgroup::getSep();
        ftPath += "FT";
        //HierarchicalPath::appendPaths(path, "/FT");
        QoSManagerInterface* qosFTManager = 0;
        qosFTManager = qosManager->createSubDomainQoSManager(ftPath, 100, m_runtimePeriod);

        String servicePath = Cgroup::getSep();
        servicePath += "service";
        //HierarchicalPath::appendPaths(path, "/service");
        QoSManagerInterface* qosServiceManager = 0;
        qosServiceManager = qosManager->createSubDomainQoSManager(servicePath, 100, m_runtimePeriod);

        //String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();
        //qosManager = m_serviceQosManager->createSubDomainQoSManager(path, 10000, QoSManagerInterface::DEFAULT_PERIOD);
    }
    sPtr->open(params, ServiceAbstract::FT_REPLICA, qosManager);
    printf("INFO: createLocalServiceReplica() SID=%s after open service!\n", sid->toString().c_str());

    ServiceInstanceInfoPtr info;
    //UUIDPtr iid = sPtr->getIID();
    this->getInstanceOfService(sid, iid, info);
    MeshPtr meshPtr;
    try {
        getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    meshPtr->onServiceCreation(info);

}

void SthenoCore::createServiceInstance(UUIDPtr& sid, UUIDPtr& iid, ServiceAbstractPtr& sPtr) throw (RuntimeException&, ServiceException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    LocalServicePtr servicePtr;
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        printf("INFO: createServiceInstance() - SID=%s not found service!\n", sid->toString().c_str());
        LocalService* ls = new LocalService(sid);
        servicePtr.reset(ls);
        if (m_serviceMap.bind(sid, servicePtr) != 0) {
            throw ServiceException("Error on binding service");
        }
    }
    printf("INFO: createServiceInstance() - SID=%s found service!\n", sid->toString().c_str());
    ServiceAbstract* i = m_serviceFactory.getInstance(sid);
    i->setIID(iid);
    ServiceAbstractPtr iPtr(i);
    servicePtr->addServiceInstance(iPtr);
    sPtr = iPtr;
}

OverlayInterface* SthenoCore::getOverlayInterface() throw (RuntimeException&) {
    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    //DiscoveryInterfacePtr diPtr(new DiscoveryInterface(m_overlay->getDiscovery()));
    DiscoveryPtr discoveryPtr;
    MeshPtr meshPtr;
    try {
        m_overlay->getDiscovery(discoveryPtr);
        m_overlay->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    DiscoveryInterfacePtr diPtr(new DiscoveryInterface(discoveryPtr));
    //MeshInterfacePtr miPtr(new MeshInterface(m_overlay->getMesh()));
    MeshInterfacePtr miPtr(new MeshInterface(meshPtr));
    return new OverlayInterface(this, diPtr, miPtr);
}

QoSManagerInterface* SthenoCore::getQoSManagerInterface(String& path, ULong runtime, ULong period,
        bool create) throw (RuntimeException&) {
    //return new QoSManagerInterface(this);
    UUIDPtr runtimeUUID;
    getUUID(runtimeUUID);
    String domainPath = "runtime_" + runtimeUUID->toString();
    if (Cgroup::FLAT_HIERARCHY) {
        if (path != "") {
            FlatHierarchicalPath::appendPaths(domainPath, path);
        }
    } else {
        HierarchicalPath::appendPaths(domainPath, path);
    }
    QoSManagerInterface* qosManager = new QoSManagerInterface(this, domainPath, runtime, period, create);
    //String domainPath = path;//"runtime_" + this->getUUID()->toString();
    //domainPath += "/p2p/mesh";
    //CPUGroupQoS qos("/HRT", domainPath, runtime, period, 1024);
    //qosManager->createGroupCPUReserve(domainPath, qos);
    return qosManager;
}

String& SthenoCore::toString() {
    m_string = "Core:";
    ACE_Hash_Map_Manager<UUIDPtr, LocalServicePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_serviceMap.begin();
    while (iter != m_serviceMap.end()) {
        //m_string += "\n\t" + iter->item()->toString();
        iter++;
    }
    return m_string;
}

void SthenoCore::onRuntimeError() {
    close();
    exit(-1);
}

QoSManager& SthenoCore::getQoSManager() throw (RuntimeException) {
    return m_qosManager;
}

QoSManagerInterface* SthenoCore::getQoSManagerForReplicationGroup(
        UUIDPtr& sid,
        UUIDPtr& rgid,
        ServiceParamsPtr& params
        ) throw (ServiceException&) {
    if (m_reservationFlag) {
        //UUIDPtr serviceCgroupUUID(UUID::generateUUID());    
        //String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();
        String path = "service_" + sid->toString() + "_" + rgid->toString();
        QoSManagerInterface* qosManager =
                m_serviceQosManager->createSubDomainQoSManager(path, 100, m_runtimePeriod);
        return qosManager;
    } else {
        return 0;
    }

}
//TODO: this is the one that is used

void SthenoCore::createReplicationGroup(
        OverlayPeerInfoPtr& primary,
        list<OverlayPeerInfoPtr>& replicas,
        UUIDPtr& rgid,
        UUIDPtr& sid,
        ServiceParamsPtr& params
        ) throw (ServiceException&) {

    if (!isValid()) {
        throw RuntimeException(RuntimeException::INVALID_RUNTIME);
    }
    ServiceAbstractPtr sPtr;
    FaultTolerancePtr ftToleranceSvc;
    try {
        m_overlay->getFaultTolerance(ftToleranceSvc);
    } catch (OverlayException& ex) {
        throw RuntimeException(RuntimeException::INVALID_OVERLAY);
    }
    printf("==========================> SID=%s FT params=%p %p\n", sid->toString().c_str(), params->getFTParams().get(), params->getQuerySpecializationBlob());
    if (!params->getFTParams().null()) {
        UUIDPtr iid(UUID::generateUUID());
        createServiceInstance(sid, iid, sPtr);

        UInt serviceOverallQoS = SERVICE_DEFAULT_QOS * 2; //5000;
        //UInt serviceFTQoS = 50000; //50000;
        //UInt serviceQoS = 50000; //50000;
        //total=31K
        //+1 = 46K
        //QoS        
        UUIDPtr serviceCgroupUUID(UUID::generateUUID());
        String path = "service_" + sid->toString() + "_" + serviceCgroupUUID->toString();
        QoSManagerInterface* qosManager = 0;
        if (m_reservationFlag) {
            //qosManager = m_serviceQosManager->createSubDomainQoSManager(path, serviceOverallQoS, m_runtimePeriod);
            qosManager = m_runtimeQoSManager;
        }
        QoSManagerInterface* qosFTManager = qosManager;
        /*String ftPath = Cgroup::getSep();
        ftPath += "FT";
        //HierarchicalPath::appendPaths(path, "/FT");        
        if (m_reservationFlag) {
            qosFTManager = qosManager->createSubDomainQoSManager(ftPath, serviceFTQoS, m_runtimePeriod);
        }*/
        QoSManagerInterface* qosServiceManager = qosManager;
        /*String servicePath = Cgroup::getSep();
        servicePath += "service";
        //HierarchicalPath::appendPaths(path, "/service");        
        if (m_reservationFlag) {
            qosServiceManager = qosManager->createSubDomainQoSManager(servicePath, serviceQoS, m_runtimePeriod);
        }*/
        ftToleranceSvc->createReplicationGroup(sPtr, params, rgid, primary, replicas, qosFTManager);
        sPtr->open(params, ServiceAbstract::FT_REPLICA, qosServiceManager);
        //free meta qos manager
        /*if (qosManager != 0) {
            delete qosManager;
        }*/
        ServiceInstanceInfoPtr info;
        //UUIDPtr iid = sPtr->getIID();
        this->getInstanceOfService(sid, iid, info);
        MeshPtr meshPtr;
        try {
            getOverlay()->getMesh(meshPtr);
        } catch (OverlayException& ex) {
            throw RuntimeException(RuntimeException::INVALID_OVERLAY);
        }
        meshPtr->onServiceCreation(info);
    } else {
        throw ServiceException(ServiceException::INVALID_FT_PARAMS);
    }

}
