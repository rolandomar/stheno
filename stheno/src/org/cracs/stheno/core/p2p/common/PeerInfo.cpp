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
 * File:   PeerInfo.cpp
 * Author: rmartins
 * 
 * Created on June 21, 2010, 7:08 PM
 */

#include "PeerInfo.h"

PeerInfo::PeerInfo() {
}

PeerInfo::PeerInfo(const PeerInfo& p) :
m_type(p.m_type), m_uuidPtr(p.m_uuidPtr), m_fidPtr(p.m_fidPtr) {
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInfoPtr, ACE_SYNCH_RW_MUTEX>::CONST_ITERATOR iter = p.m_serviceMap.begin();
    while (iter != p.m_serviceMap.end()) {
        m_serviceMap.bind(iter->key(), iter->item());
        iter++;
    }
}

PeerInfo::PeerInfo(InputStream& inputStream) {
    this->deserialize(inputStream);
}

PeerInfo::PeerInfo(UInt type, const UUIDPtr& uuidPtr, const UUIDPtr& fidPtr, const SAPInfoPtr& meshSAP,
        const SAPInfoPtr& discoverySAP,
        const SAPInfoPtr& ftSAP) :
m_type(type), m_uuidPtr(uuidPtr), m_fidPtr(fidPtr), m_meshSAP(meshSAP), m_ftSAP(ftSAP), m_discoverySAP(discoverySAP) {
}

PeerInfo::~PeerInfo() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)~PeerInfo(%@)\n"), this));
}

bool PeerInfo::addService(const ServiceInfoPtr& service) {
    if (m_serviceMap.find(service->getSid()) == -1) {
        if (m_serviceMap.bind(service->getSid(), service) == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)BindService OK\n")));
            return true;
        }
    }
    return false;
}

bool PeerInfo::addService(const UUIDPtr& sid) {
    if (m_serviceMap.find(sid) == -1) {
        ServiceInfoPtr serviceInfo(new ServiceInfo(sid));
        return addService(serviceInfo);
    }
    return false;
}

bool PeerInfo::removeService(const UUIDPtr& sid) {
    if (m_serviceMap.find(sid) == -1) {
        return false;
    }
    if (m_serviceMap.unbind(sid) == 0) {
        return true;
    }
    return false;
}

bool PeerInfo::removeService(const UUIDPtr& sid, const UUIDPtr& iid) {
    ServiceInfoPtr info;
    if (m_serviceMap.find(sid, info) == -1) {
        return false;
    }
    if (!info->removeServiceInstance(iid)) {
        return false;
    }
    if (info->getInstanceCount() == 0) {
        m_serviceMap.unbind(sid);
    }
    return true;
}

bool PeerInfo::changeServiceIID(const UUIDPtr& sid, const UUIDPtr& iid, UUIDPtr& newIdd) throw (ServiceException&) {
    ServiceInfoPtr info;
    if (m_serviceMap.find(sid, info) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: PeerInfo::changeServiceIID() - failed to find service(%@)\n"), this));
        return false;
    }

    return info->setIID(iid, newIdd);
}

bool PeerInfo::addServiceInstance(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& sapInfo/*list<EndpointPtr>& list*/) {
    ServiceInfoPtr info;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: PeerInfo::addServiceInstance - SID=%s\n"), sid->toString().c_str()));
    if (m_serviceMap.find(sid, info) == -1) {
        info.reset(new ServiceInfo(sid));
        if (!addService(info)) {
            return false;
        }
    }
    return info->addServiceInstance(iid, sapInfo);
}

bool PeerInfo::addServiceInstance(const UUIDPtr& sid, const UUIDPtr& iid) {
    ServiceInfoPtr info;
    if (m_serviceMap.find(sid, info) == -1) {
        info.reset(new ServiceInfo(sid));
        if (!addService(info)) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Failed Service bind\n")));
            return false;
        }
        ///ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)KKKKKK123213\n")));
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)KKKKKK54444444\n")));
    return info->addServiceInstance(iid);
}

const UUIDPtr& PeerInfo::getPID() {
    return m_uuidPtr;
}

const UUIDPtr& PeerInfo::getFID() {
    return m_fidPtr;
}

SAPInfoPtr& PeerInfo::getDiscoverySAP() {
    return m_discoverySAP;
}

SAPInfoPtr& PeerInfo::getMeshSAP() {
    return m_meshSAP;
}

SAPInfoPtr& PeerInfo::getFTSAP() {
    return m_ftSAP;
}

UInt PeerInfo::getType() {
    return m_type;
}

/**
 * Checks if a sid has any iid active
 * @param iid
 * @return
 */
bool PeerInfo::isServiceRunning(const UUIDPtr& sid) {
    if (m_serviceMap.find(sid) == 0) {
        return true;
    }
    return false;
}

bool PeerInfo::isServiceInstanceRunning(const UUIDPtr& sid, const UUIDPtr& iid) {
    ServiceInfoPtr servicePtr;
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        //printf("PeerInfo:isServiceInstanceRunning()  - no sid! (%s) iid=%s\n", sid->toString().c_str(), iid->toString().c_str());
        return false;
    }
    //printf("PeerInfo:isServiceInstanceRunning()  - SID FOUND! pid=%s\n", this->m_uuidPtr->toString().c_str());
    return servicePtr->isServiceInstanceRunning(iid);
}

bool PeerInfo::getInstanceOfService(const UUIDPtr& sid, const UUIDPtr& iid, ServiceInstanceInfoPtr& instance)
throw (ServiceException&) {
    ServiceInfoPtr servicePtr;

    if (m_serviceMap.find(sid, servicePtr) == -1) {
        throw ServiceException("Service not found!");
    }
    return servicePtr->getInstanceOfService(iid, instance);

}

void PeerInfo::getInstancesOfService(const UUIDPtr& sid, list<UUIDPtr>& l) throw (ServiceException&) {
    ServiceInfoPtr servicePtr;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)S=%s\n"), sid->toString().c_str()));
    if (m_serviceMap.find(sid, servicePtr) == -1) {
        throw ServiceException("Service not found!");
    }
    servicePtr->getInstancesOfService(l);
}

void PeerInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_type);
    m_uuidPtr->serialize(outputStream);
    m_fidPtr->serialize(outputStream);
    if (!m_meshSAP.null()) {
        outputStream.write_boolean(true);
        m_meshSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    if (!m_discoverySAP.null()) {
        outputStream.write_boolean(true);
        m_discoverySAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    if (!m_ftSAP.null()) {
        outputStream.write_boolean(true);
        m_ftSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    int count = m_serviceMap.current_size();
    outputStream.write_long(count);
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_serviceMap.begin();
    while (iter != m_serviceMap.end()) {
        iter->item()->serialize(outputStream);
        iter++;
    }


}

void PeerInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_type);
    UUID* peerID = new UUID(inputStream);
    m_uuidPtr.reset(peerID);
    UUID* fID = new UUID(inputStream);
    m_fidPtr.reset(fID);
    Boolean flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* e = new SAPInfo(inputStream);
        m_meshSAP.reset(e);
    } else {
        m_meshSAP.reset(0);
    }

    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* e = new SAPInfo(inputStream);
        m_discoverySAP.reset(e);
    } else {
        m_discoverySAP.reset(0);
    }

    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* e = new SAPInfo(inputStream);
        m_ftSAP.reset(e);
    } else {
        m_ftSAP.reset(0);
    }
    //services
    m_serviceMap.unbind_all();
    int count = 0;
    inputStream.read_long(count);
    for (int i = 0; i < count; i++) {
        ServiceInfo* pi = new ServiceInfo(inputStream);
        ServiceInfoPtr iPtr(pi);
        m_serviceMap.bind(pi->getSid(), iPtr);
    }
}

String& PeerInfo::toString() {
    stringstream ss;
    //m_string = "PeerInfo(" + m_uuidPtr->toString() + "," + m_fidPtr->toString() + ")";
    ss << "PeerInfo(" << m_uuidPtr->toString() << "," << m_fidPtr->toString() << ")";
    ACE_Hash_Map_Manager<UUIDPtr, ServiceInfoPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_serviceMap.begin();
    while (iter != m_serviceMap.end()) {
        //m_string += "\n\t" + iter->item()->toString();
        ss << "\n\t" + iter->item()->toString();
        iter++;
    }
    m_string = ss.str().c_str();
    return m_string;
}

UInt PeerInfo::getTotalServicesRunning() {
    UInt count = 0;
    ServiceMap::iterator iter = m_serviceMap.begin();
    while (iter != m_serviceMap.end()) {
        count += iter->item()->getInstanceCount();
        iter++;
    }
    return count;
}