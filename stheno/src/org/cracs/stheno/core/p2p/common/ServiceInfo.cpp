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
 * File:   ServiceInfo.cpp
 * Author: rmartins
 * 
 * Created on June 21, 2010, 11:47 AM
 */

#include "ServiceInfo.h"
#include <stheno/core/p2p/common/PeerInfo.h>

ServiceInfo::ServiceInfo(const UUIDPtr& sid) :
m_sid(sid) {
}

ServiceInfo::ServiceInfo(const ServiceInfo& s) :
m_sid(s.m_sid) {
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::CONST_ITERATOR iter = s.m_runningServicesMap.begin();
    while (iter != s.m_runningServicesMap.end()) {
        m_runningServicesMap.bind(iter->key(), iter->item());
        iter++;
    }
}

ServiceInfo::ServiceInfo(InputStream& inputStream) {
    deserialize(inputStream);
}

/*ServiceInfo::ServiceInfo(PeerInfo* peer,UUIDPtr& sid) :
m_peer(peer),m_sid(sid) {
}*/

ServiceInfo::~ServiceInfo() {
   // ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: ServiceInfo::~ServiceInfo(%@) %s\n"), this,m_sid->toString().c_str()));
}

bool ServiceInfo::addServiceInstance(ServiceInstanceInfoPtr& instance) {
    if (m_runningServicesMap.bind(instance->getIID(), instance) == 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: ServiceInstane::addServiceInstance() ok\n")));
        return true;
    }
    return false;
}

bool ServiceInfo::addServiceInstance(const UUIDPtr& iid, SAPInfoPtr& info/*list<EndpointPtr>& list*/) {
    if (m_runningServicesMap.find(iid) == 0) {
        return false;
    }
    //ServiceInstanceInfoPtr serviceInstance(new ServiceInstanceInfo(m_sid,iid, list));
    ServiceInstanceInfoPtr serviceInstance(new ServiceInstanceInfo(m_sid, iid, info));
    return addServiceInstance(serviceInstance);
}

bool ServiceInfo::addServiceInstance(const UUIDPtr& iid) {
    if (m_runningServicesMap.find(iid) == 0) {
        return false;
    }
    ServiceInstanceInfoPtr serviceInstance(new ServiceInstanceInfo(m_sid, iid));
    return addServiceInstance(serviceInstance);
}

bool ServiceInfo::removeServiceInstance(const UUIDPtr& iid) {
    if (m_runningServicesMap.unbind(iid) == 0) {
        return true;
    }
    return false;
}

UUIDPtr& ServiceInfo::getSid() {
    return m_sid;
}

bool ServiceInfo::setIID(const UUIDPtr& oldIid, UUIDPtr& newIdd) throw (ServiceException&) {
    ServiceInstanceInfoPtr info;
    if (m_runningServicesMap.unbind(oldIid, info) != 0) {
        return false;
    }

    info->setIID(newIdd);
    if (m_runningServicesMap.bind(newIdd, info) != 0) {
        return false;
    }

    return true;
}

bool ServiceInfo::isServiceInstanceRunning(const UUIDPtr& iid) {
    if (m_runningServicesMap.find(iid) == 0) {
        //printf("ServiceInfo::isServiceInstanceRunning: Service is Running!\n");
        return true;
    }
    //printf("ServiceInfo::isServiceInstanceRunning: not runnig? - \n%s\n", toString().c_str());

    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {

        /*printf("ServiceInfo::isServiceInstanceRunning: seach=%s item %s\n",
                iid->toString().c_str(),
                iter->item()->getIID()->toString().c_str()
                );*/
        iter++;
    }

    return false;
}

bool ServiceInfo::getInstanceOfService(const UUIDPtr& iid, ServiceInstanceInfoPtr& service) throw (ServiceException&) {
    if (m_runningServicesMap.find(iid, service) == 0) {
        return true;
    }
    throw ServiceException("Not exists!");
}

void ServiceInfo::getInstancesOfService(list<UUIDPtr>& l) {
    //list<UUIDPtr>* l = new list<UUIDPtr>();
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        UUIDPtr& iid = iter->item()->getIID();
        l.push_back(iid);
        iter++;
    }
    //return l;
}

void ServiceInfo::getInstancesOfServiceInfo(list<ServiceInstanceInfoPtr>& l) {
    //list<ServiceInstanceInfoPtr>* l = new list<ServiceInstanceInfoPtr>();
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        //list<EndpointPtr> el;
        //iter->item()->getEndpointList(el);
        //ServiceInstanceInfo* si = new ServiceInstanceInfo(iter->item()->getSID(),iter->item()->getIID(),el);
        //ServiceInstanceInfoPtr siPtr(si);
        //l.push_back(siPtr);
        l.push_back(iter->item());
        iter++;
    }
    //return l;
}

void ServiceInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    int count = m_runningServicesMap.current_size();
    outputStream.write_long(count);
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        iter->item()->serialize(outputStream);
        iter++;
    }


}

void ServiceInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_sid.reset(uuid);
    m_runningServicesMap.unbind_all();
    int count = 0;
    inputStream.read_long(count);
    for (int i = 0; i < count; i++) {
        ServiceInstanceInfo* pi = new ServiceInstanceInfo(inputStream);
        ServiceInstanceInfoPtr iPtr(pi);
        m_runningServicesMap.bind(pi->getIID(), iPtr);
    }
}

String& ServiceInfo::toString() {
    stringstream ss;
    //m_string = "ServiceInfo(" + m_sid->toString() + ")";
    /*ss << "ServiceInfo(" << m_sid->toString() << ")";
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInstanceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        //m_string += "\n\t\t" + iter->item()->toString();
        ss << "\n\t\t" << iter->item()->toString();
        iter++;
    }
    m_string = ss.str().c_str();*/
    return m_string;
}
