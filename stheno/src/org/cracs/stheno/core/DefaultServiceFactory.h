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
 * File:   DefaultServiceFactory.h
 * Author: rmartins
 *
 * Created on August 2, 2010, 12:07 PM
 */

#ifndef DEFAULTSERVICEFACTORY_H
#define	DEFAULTSERVICEFACTORY_H

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

#include <stheno/core/ServiceFactory.h>

class DefaultServiceFactory: public ServiceFactory {
public:
    DefaultServiceFactory();    
    virtual ~DefaultServiceFactory();
    
   virtual ServiceAbstract* getInstance(const UUIDPtr& sid) throw(ServiceException&);      
   virtual bool addService(ServiceAbstract* serviceImpl);   
   virtual UUIDPtr& getServiceName(const char* name) throw (ServiceException&);
   
protected:
    ACE_Hash_Map_Manager<UUIDPtr, ServiceAbstract*, ACE_SYNCH_RW_MUTEX> m_services;
};

#endif	/* DEFAULTSERVICEFACTORY_H */

