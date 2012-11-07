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
 * File:   SthenoCoreInterface.h
 * Author: rmartins
 *
 * Created on October 21, 2010, 4:33 PM
 */

#ifndef STHENOCOREINTERFACE_H
#define	STHENOCOREINTERFACE_H

#include <euryale/common/uuid/UUID.h>
#include <stheno/service/UserService.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/service/ServiceClient.h>
#include <stheno/exception/RuntimeException.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>
#include <stheno/core/DefaultServiceFactory.h>
#include <stheno/core/LocalService.h>
#include <euryale/qos/RTParams.h>

//#include <stheno/core/SthenoCore.h>
class SthenoCore;
#include <stheno/core/QoSManagerInterface.h>

class SthenoCoreInterface {
public:
    SthenoCoreInterface(SthenoCore* core);
    virtual ~SthenoCoreInterface();
        
    void createLocalService(UUIDPtr& sid, ServiceParamsPtr& params,UUIDPtr& iid) throw (ServiceException&);   
        
    void createReplicationGroup(
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& peers,
            UUIDPtr& rgid,
            UUIDPtr& sid,            
            ServiceParamsPtr& params            
            ) throw (ServiceException&);
    
    void createLocalServiceReplica(UUIDPtr& sid, ServiceParamsPtr& params,ServiceAbstractPtr& sPtr) throw (ServiceException&);       
    void changeIIDOfService(UUIDPtr& sid,UUIDPtr& iid,UUIDPtr& newIid) throw (ServiceException&);
        
    void stopLocalService(UUIDPtr& sid, UUIDPtr& iid) throw (ServiceException&);    
    bool isServiceRunning(UUIDPtr& sid, UUIDPtr& idd);
    void getInstancesOfService(UUIDPtr& sid,UUIDPtr& iid,ServiceInstanceInfoPtr& info) throw (ServiceException&);
    list<UUIDPtr>* getInstancesOfServiceUUIDs(UUIDPtr& sid) throw (ServiceException&);
    list<ServiceInstanceInfoPtr>* getInstancesOfService(UUIDPtr& sid) throw (ServiceException&);
    //void allocateQoS(QoSResources* qos) throw (ServiceException&);
    ServiceFactory& getDefaultServiceFactory();
        
    void getUUID(UUIDPtr& uuid);
    bool isValid();
    bool isSuspended();
    bool isClose();
    Byte getStatus();
    
    UInt getRuntimeQoS();
    UInt getRuntimeQoSPeriod();
    //QoSManagerInterface* getQoSManager(String& path, ULong runtime,ULong period);
    
private:
    SthenoCore* m_core;
    QoSManagerInterface* m_qosManager;
};

#endif	/* STHENOCOREINTERFACE_H */

