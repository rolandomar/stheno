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
 * File:   LocalService.h
 * Author: rmartins
 *
 * Created on August 2, 2010, 2:48 PM
 */

#ifndef LOCALSERVICE_H
#define	LOCALSERVICE_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/Types.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>
#include <stheno/service/exception/ServiceException.h>
#include <stheno/service/ServiceAbstract.h>
#include <stheno/core/ServiceFactory.h>

class LocalService {
public:        
    LocalService(UUIDPtr& sid);            
    virtual ~LocalService();
    
    void close();    
    UUIDPtr& getSid();          
    void setIID(const UUIDPtr& iid,UUIDPtr& newIdd) throw(ServiceException&);
    bool existsRunningService(UUIDPtr& iid);
    UInt getInstanceCount();
    bool addServiceInstance(ServiceAbstractPtr& instance);    
    void removeServiceInstance(UUIDPtr& iid) throw(ServiceException&);
    bool getInstancesOfServiceInfo(UUIDPtr& iid,ServiceInstanceInfoPtr& info);
    //service is a out arg.
    bool getInstanceOfService(UUIDPtr& iid,ServiceAbstractPtr& service) throw(ServiceException&);    
    list<UUIDPtr>* getInstancesOfService();    
    list<ServiceInstanceInfoPtr>* getInstancesOfServiceInfo();
    String& toString();

protected:    
    UUIDPtr m_sid;
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstractPtr, ACE_SYNCH_RW_MUTEX> m_runningServicesMap;
    String m_string;        
};

typedef ACE_Strong_Bound_Ptr<LocalService, ACE_SYNCH_MUTEX> LocalServicePtr;
#endif	/* LOCALSERVICE_H */

