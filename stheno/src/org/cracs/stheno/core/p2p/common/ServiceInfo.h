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
 * File:   ServiceInfo.h
 * Author: rmartins
 *
 * Created on June 21, 2010, 11:47 AM
 */

#ifndef SERVICEINFO_H
#define	SERVICEINFO_H

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
/*
 *  Manager for all services
 */
class PeerInfo;
//typedef ACE_Strong_Bound_Ptr<PeerInfo, ACE_SYNCH_MUTEX> PeerInfoPtr;

class ServiceInfo: public Serializable {
public:
    
    //Overlay direct use
    ServiceInfo(const UUIDPtr& sid);
    ServiceInfo(const ServiceInfo& s);
    ServiceInfo(InputStream& inputStream);
    
    //Map usage for external peers+services
    //ServiceInfo(PeerInfo* peer, UUIDPtr& sid);
    virtual ~ServiceInfo();

    UUIDPtr& getSid();    
    bool setIID(const UUIDPtr& oldIid,UUIDPtr& newIdd) throw(ServiceException&);            

    bool existsRunningService(const UUIDPtr& iid) {
        ServiceInstanceInfoPtr service;
        if (m_runningServicesMap.find(iid, service) == 0) {
            return true;
        }
        return false;
    }

    UInt getInstanceCount() {
        return m_runningServicesMap.current_size();
    }
       
    bool addServiceInstance(ServiceInstanceInfoPtr& instance);
    //bool addServiceInstance(const UUIDPtr& iid,list<EndpointPtr>& list);
    bool addServiceInstance(const UUIDPtr& iid,SAPInfoPtr& info);
    bool addServiceInstance(const UUIDPtr& iid);


    bool removeServiceInstance(const UUIDPtr& iid);


    bool isServiceInstanceRunning(const UUIDPtr& iid);
    
    bool getInstanceOfService(const UUIDPtr& iid,ServiceInstanceInfoPtr& service) throw(ServiceException&);

    void getInstancesOfService(list<UUIDPtr>& l);
    
    void getInstancesOfServiceInfo(list<ServiceInstanceInfoPtr>& l);
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);    
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    String& toString();

protected:
    //PeerInfo* m_peer;
    UUIDPtr m_sid;
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX> m_runningServicesMap;
    String m_string;    
};

typedef ACE_Strong_Bound_Ptr<ServiceInfo, ACE_SYNCH_MUTEX> ServiceInfoPtr;

#endif	/* SERVICEINFO_H */

