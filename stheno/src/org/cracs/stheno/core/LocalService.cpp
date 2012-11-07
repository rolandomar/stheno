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
 * File:   LocalService.cpp
 * Author: rmartins
 * 
 * Created on August 2, 2010, 2:48 PM
 */

#include "LocalService.h"

#include <stheno/core/p2p/common/PeerInfo.h>

LocalService::LocalService(UUIDPtr& sid) : m_sid(sid) {
}

LocalService::~LocalService() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)~ServiceInfo(%s)\n"), m_sid->toString().c_str()));
}

bool LocalService::addServiceInstance(ServiceAbstractPtr& instance) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addServiceInstance() - IID=%s\n"),instance->getIID()->toString().c_str()));
    UUIDPtr iid;
    instance->getIID(iid);
    if (m_runningServicesMap.bind(iid, instance) == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addServiceInstance() - ServiceInstane ok\n")));
        return true;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: addServiceInstance() - ServiceInstane nok\n")));
    return false;
}

void LocalService::removeServiceInstance(UUIDPtr& iid) throw (ServiceException&) {
    ServiceAbstractPtr sPtr;
    if (m_runningServicesMap.unbind(iid, sPtr) == 0) {
        sPtr->close();
        return;
    }
    throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
}

void LocalService::close() {
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstractPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        try {
            iter->item()->close();
        } catch (ServiceException& ex) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)LocalService::close: error!\n")));
        }
        iter++;
    }
}

bool LocalService::getInstanceOfService(UUIDPtr& iid, ServiceAbstractPtr& service) throw (ServiceException&) {
    if (m_runningServicesMap.find(iid, service) == 0) {
        return true;
    }
    throw ServiceException("Not exists!");
}
//get list of IID

list<UUIDPtr>* LocalService::getInstancesOfService() {
    list<UUIDPtr>* l = new list<UUIDPtr > ();
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstractPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        UUIDPtr iid;
        iter->item()->getIID(iid);
        l->push_back(iid);
        iter++;
    }
    return l;
}

bool LocalService::getInstancesOfServiceInfo(UUIDPtr& iid, ServiceInstanceInfoPtr& info) {
    ServiceAbstractPtr service;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LocalService::getInstancesOfServiceInfo() IID=%s\n"),iid->toString().c_str()));
    if (m_runningServicesMap.find(iid, service) == 0) {
        UUIDPtr sid;
        service->getSID(sid);
        UUIDPtr iid;
        service->getIID(iid);
        SAPInfoPtr sap;
        service->getSAP(sap);
        ServiceInstanceInfo* si = new ServiceInstanceInfo(sid, iid, sap);
        info.reset(si);
        //Endpoint e;
        //info->getSAPInfo()->getFirstEndpoint(e);       
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LocalService::getInstancesOfServiceInfo() exists!\n")));        
        return true;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Error: LocalService::getInstancesOfServiceInfo() - Error! (%s)\n"),
            iid->toString().c_str()
            ));
    return false;
}

list<ServiceInstanceInfoPtr>* LocalService::getInstancesOfServiceInfo() {
    list<ServiceInstanceInfoPtr>* l = new list<ServiceInstanceInfoPtr > ();
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstractPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        UUIDPtr sid;
        iter->item()->getSID(sid);
        UUIDPtr iid;
        iter->item()->getIID(iid);
        SAPInfoPtr sap;
        iter->item()->getSAP(sap);
        ServiceInstanceInfo* si = new ServiceInstanceInfo(sid,
                iid, sap);
        ServiceInstanceInfoPtr siPtr(si);
        l->push_back(siPtr);
        iter++;
    }
    return l;
}

String& LocalService::toString() {
    m_string = "LocalService(" + m_sid->toString() + ")";
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstractPtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_runningServicesMap.begin();
    while (iter != m_runningServicesMap.end()) {
        //m_string += "\n\t\t" + iter->item()->toString();
        iter++;
    }
    return m_string;
}

UUIDPtr& LocalService::getSid() {
    return m_sid;
}

void LocalService::setIID(const UUIDPtr& iid, UUIDPtr& newIid) throw (ServiceException&) {
    ServiceAbstractPtr service;
    if (m_runningServicesMap.unbind(iid, service) != 0) {
        throw ServiceException("Service not fount!");
    }
    service->setIID(newIid);
    if (m_runningServicesMap.bind(newIid, service) != 0) {
        throw ServiceException("Failed to change IID!");
    }
}

bool LocalService::existsRunningService(UUIDPtr& iid) {
    ServiceAbstractPtr service;
    if (m_runningServicesMap.find(iid, service) == 0) {
        return true;
    }
    return false;
}

UInt LocalService::getInstanceCount() {
    return m_runningServicesMap.current_size();
}