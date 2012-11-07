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
 * File:   ServiceClient.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 29, 2010, 3:59 PM
 */

#ifndef SERVICECLIENT_H
#define	SERVICECLIENT_H
#include <euryale/common/uuid/UUID.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include "ClientParams.h"

class Stheno;
/**
 * Proxy pattern
 */
class ServiceClient {
public:
    virtual ~ServiceClient();
    friend class Stheno;
    //client startup
    virtual int open(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp) = 0;
    //client shutdown/close
    virtual void close();
    //has to have the same SID of the service it belongs
    virtual const UUIDPtr& getSID() = 0;
    //runtime
    virtual Stheno* getRuntime();
    
protected:
    Stheno* m_runtime;
    //ClientParamsPtr m_cp;
    ServiceClient(Stheno* runtime);
    


};

typedef ACE_Strong_Bound_Ptr<ServiceClient, ACE_Recursive_Thread_Mutex> ServiceClientPtr;

#endif	/* SERVICECLIENT_H */

