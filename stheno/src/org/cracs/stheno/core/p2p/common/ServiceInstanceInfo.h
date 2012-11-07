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
 * File:   ServiceInstanceInfo.h
 * Author: rmartins
 *
 * Created on June 21, 2010, 11:49 AM
 */

#ifndef SERVICEINSTANCEINFO_H
#define	SERVICEINSTANCEINFO_H
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
#include <stheno/service/exception/ServiceException.h>
#include <euryale/net/endpoint/SAPInfo.h>

class ServiceInfo;

class ServiceInstanceInfo: public Serializable{
public:
    ServiceInstanceInfo(const ServiceInstanceInfo& si);
    ServiceInstanceInfo(const UUIDPtr& sid,const UUIDPtr& iid);
    ServiceInstanceInfo(const UUIDPtr& sid,const UUIDPtr& iid,SAPInfoPtr& sapInfo);
    ServiceInstanceInfo(InputStream& inputStream);
     
    virtual ~ServiceInstanceInfo();

    String& toString();

    UUIDPtr& getIID();
    void setIID(UUIDPtr& iid);
    
    UUIDPtr& getSID();
    
    SAPInfoPtr& getSAPInfo();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);    
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    SAPInfoPtr m_sapInfo;
    String m_string;
};
typedef ACE_Strong_Bound_Ptr<ServiceInstanceInfo,ACE_SYNCH_MUTEX> ServiceInstanceInfoPtr;

#endif	/* SERVICEINSTANCEINFO_H */

