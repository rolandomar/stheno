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
 * File:   DefaultServiceFactory.cpp
 * Author: rmartins
 * 
 * Created on August 2, 2010, 12:07 PM
 */

#include "DefaultServiceFactory.h"
#include <euryale/common/ListHelper.h>

DefaultServiceFactory::DefaultServiceFactory() {
}

DefaultServiceFactory::~DefaultServiceFactory() {
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstract*,
            ACE_SYNCH_RW_MUTEX>::iterator iter = m_services.begin();
    while (iter != m_services.end()) {
        delete iter->item();
        iter++;
    }
    m_services.unbind_all();
}

ServiceAbstract* DefaultServiceFactory::getInstance(const UUIDPtr& sid) throw (ServiceException&) {
    ServiceAbstract* service = 0;
    if (m_services.find(sid, service) == -1) {
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    if (service == 0) {
        //paranoid check
        throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
    }
    return service->duplicate();
}

bool DefaultServiceFactory::addService(ServiceAbstract* serviceImpl) {
    UUIDPtr sid;
    serviceImpl->getSID(sid);
    if (m_services.find(sid) == -1) {
        if (m_services.bind(sid, serviceImpl) == 0) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)DefaultServiceFactory:BindService OK\n")));
            return true;
        }
    }
    return false;
}

UUIDPtr& DefaultServiceFactory::getServiceName(const char* name) throw (ServiceException&) {
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstract*,
            ACE_SYNCH_RW_MUTEX>::iterator iter = m_services.begin();
    while (iter != m_services.end()) {
        if (ACE_OS::strcmp(iter->item()->getName(),name) == 0) {
            return iter->key();
        }
        iter++;
    }
    throw ServiceException(ServiceException::SERVICE_NOT_KNOWN);
}