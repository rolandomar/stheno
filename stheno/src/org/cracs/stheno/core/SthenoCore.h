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
 * File:   SthenoCore.h
 * Author: rmartins
 *
 * Created on July 27, 2010, 3:02 PM
 */

#ifndef STHENOCORE_H
#define	STHENOCORE_H

#include <ace/INET_Addr.h>
#include <ace/ARGV.h>

#include <euryale/common/uuid/UUID.h>
#include <stheno/service/UserService.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/service/ClientParams.h>
#include <stheno/service/ServiceClient.h>
#include <stheno/exception/RuntimeException.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>
#include <stheno/core/DefaultServiceFactory.h>
#include <stheno/core/LocalService.h>
#include <euryale/qos/RTParams.h>
#include <stheno/core/SthenoCoreSignalHandler.h>
#include <stheno/core/QoSManager.h>
#include <stheno/core/QoSManagerInterface.h>
#include <stheno/common/LogStatistics.h>
#include <stheno/common/TraceRuntime.h>

class Overlay;

class SthenoCore;
typedef ACE_Strong_Bound_Ptr<SthenoCore, ACE_Recursive_Thread_Mutex> SthenoCorePtr;
class SthenoCoreInterface;
class OverlayInterface;

class SthenoCore {
public:

  static const int RUNTIME_DEFAULT_QOS = 1000;//10;//5000;
    
    static const int P2P_DEFAULT_QOS = 135;//135000;
    static const int SERVICE_DEFAULT_QOS = 50;//50000;    
    static const int SERVICE_MANAGER_DEFAULT_QOS = 5;//000;
    
    
    friend class UserService;
    friend class QoSManager;
    friend class SthenoCoreInterface;
    friend class QoSManagerInterface;
    virtual ~SthenoCore();
    //Singleton access points
    static void createRuntime(UUIDPtr& rid, SthenoCorePtr& core) throw (RuntimeException&);
    static void createRuntime(SthenoCorePtr& core) throw (RuntimeException&);

    //must pass arguments here
    void open(ACE_ARGV& args) throw (RuntimeException&);
    void close();

    bool isServiceRunning(UUIDPtr& sid, UUIDPtr& idd) throw (RuntimeException&);
    void getInstanceOfService(UUIDPtr& sid, UUIDPtr& iid, ServiceInstanceInfoPtr& info) throw (RuntimeException&, ServiceException&);
    list<UUIDPtr>* getInstancesOfServiceUUIDs(UUIDPtr& sid) throw (RuntimeException&, ServiceException&);
    list<ServiceInstanceInfoPtr>* getInstancesOfService(UUIDPtr& sid) throw (RuntimeException&, ServiceException&);


    //Auto sid deployment (cloud computing style:))
    void startService(const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid,UUIDPtr& deployUUIDD) throw (RuntimeException&, ServiceException&);
    //Deploy sid @ peerID    
    void startService(UUIDPtr& sid, UUIDPtr& peerID, ServiceParamsPtr& params, UUIDPtr& iid) throw (RuntimeException&, ServiceException&);
    //local!
    //must perform a qos check to see if we have the necessary resources
    virtual void createLocalService(UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid) throw (RuntimeException&, ServiceException&);
    virtual void createLocalServiceReplica(UUIDPtr& sid, ServiceParamsPtr& params, ServiceAbstractPtr& sPtr) throw (RuntimeException&, ServiceException&);
    virtual void changeIIDOfService(UUIDPtr& sid, UUIDPtr& iid, UUIDPtr& newIid) throw (RuntimeException&, ServiceException&);
    //stop local 
    void stopLocalService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&);
    //stop sid @ peerID (iid->(sid,peerID))    
    void stopService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&);
    //search repo for UUID
    const UUIDPtr& getSID(const char* name) throw (RuntimeException&, ServiceException&);

    //Client
    //could be 1-1,1-n (MapReduce),etc,etc
    //Entry point for a client to a particular service
    //we cannot do this, it is too inflexible
    //we should get a client, and then using a open()
    //method with the proper semantics to fit the particular
    ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& clientParams) 
            throw (RuntimeException&, ServiceException&);
    ServiceClient* getClient(const UUIDPtr& sid, const UUIDPtr& iid,ClientParamsPtr& clientParams) 
            throw (RuntimeException&, ServiceException&);
    
    void getUUID(UUIDPtr& uuid);

    bool isValid();
    bool isSuspended();
    bool isClose();
    Byte getStatus();

    OverlayInterface* getOverlayInterface() throw (RuntimeException&);
    QoSManagerInterface* getQoSManagerInterface(String& path, ULong runtime, ULong period,bool create = true) throw (RuntimeException&);
    virtual void attachOverlay(Overlay* overlay) throw (RuntimeException&);
    ServiceFactory& getServiceFactory();

    //FT callback
    QoSManagerInterface* getQoSManagerForReplicationGroup(
            UUIDPtr& sid,
            UUIDPtr& rgid,
            ServiceParamsPtr& params
            ) throw (ServiceException&);
    
    void createReplicationGroup(
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& peers,
            UUIDPtr& rgid,
            UUIDPtr& sid,            
            ServiceParamsPtr& params            
            ) throw (ServiceException&);

    String& toString();

    bool trace();
    
    UInt getRuntimeQoS(){
        return m_runtimeQoS;
    }
    
    UInt getRuntimeQoSPeriod(){
        return m_runtimePeriod;
    }
    
protected:
    // m_signalHandler;
    bool m_trace;
    bool m_reservationFlag;
    int m_runNO;
    int m_testNO;
    UInt m_runtimeQoS;
    UInt m_runtimePeriod;
    UInt m_servicesQoS;
    UInt m_maxServicesQoS;
    UInt m_perServicesQoS;
    UInt m_p2pQoS;

    enum {
        NOT_INIT = 0x1,
        RUNNING,
        SUSPENDED,
        CLOSED
    };

    ACE_SYNCH_RECURSIVE_MUTEX m_lock;

    SthenoCore();
    SthenoCore(UUIDPtr& rid);

    //QoS
    void initRuntimeQoS(ACE_ARGV& args) throw (RuntimeException&);
    void initLogging(const char* statsFile,const char* tracefile,const char* debugfile) throw (RuntimeException&);


    /*
     * local service manag. implemented     
     */
    bool addService(ServiceAbstractPtr& serviceInstance) throw (RuntimeException&, ServiceException&);
    void removeService(UUIDPtr& sid) throw (RuntimeException&, ServiceException&);
    void removeService(UUIDPtr& sid, UUIDPtr& iid) throw (RuntimeException&, ServiceException&);
    bool getInstanceOfService(UUIDPtr& sid, UUIDPtr& iid, ServiceAbstractPtr& instance)
    throw (RuntimeException&, ServiceException&);


    /*
     * End of local service manag.
     */

    //local services
    //Callback from Service
    //must remove local and notify mesh
    void onServiceClose(UserService* service);
    void onRuntimeError();

    Byte m_status;

    void toStart();
    void toSuspend();
    void toClose();

    Overlay* getOverlay() throw (RuntimeException&);
    void createServiceInstance(UUIDPtr& sid, UUIDPtr& iid,ServiceAbstractPtr& sPtr) throw (RuntimeException&, ServiceException&);
    

    QoSManager& getQoSManager() throw (RuntimeException);

    //runtime uuid
    UUIDPtr m_peerID;
    Overlay* m_overlay;
    ACE_Hash_Map_Manager<UUIDPtr, LocalServicePtr, ACE_SYNCH_RW_MUTEX> m_serviceMap;
    DefaultServiceFactory m_serviceFactory;
    QoSManager m_qosManager;
    QoSManagerInterface* m_runtimeQoSManager;
    QoSManagerInterface* m_serviceQosManager;
    String m_string;
    ACE_OSTREAM_TYPE *m_debugStream;
    //logging

};

#endif	/* STHENOCORE_H */

