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
 * Service API
 * Extend the class to develop new service
 * Must use core to ask for addition/resources/...
 */
#ifndef SERVICE_H_
#define SERVICE_H_

#include <ace/Bound_Ptr.h>
#include <stheno/service/ServiceAbstract.h>
#include <stheno/service/ClientParams.h>
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/service/ReplicationMessage.h>
#include <stheno/service/ServiceClient.h>

class Stheno;
typedef ACE_Strong_Bound_Ptr<Stheno, ACE_Recursive_Thread_Mutex> SthenoPtr;

class UserService : public ServiceAbstract {
public:
    friend class Stheno;
    friend class Overlay;  
        
    UserService(Stheno* runtime,const char* id);    
    UserService(Stheno* runtime,const char* id,UUIDPtr& iid);    
    virtual ~UserService();

    virtual void close() = 0;

    virtual Stheno* getRuntime();
           
    //get a client to this service => local bind
    //virtual ServiceClient* getClient() = 0;
    //remote binds
    virtual ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& cp) = 0;    
    virtual ServiceClient* getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp) = 0;
        
protected:      
    Stheno* m_runtime;    
};

typedef ACE_Strong_Bound_Ptr<UserService, ACE_Recursive_Thread_Mutex> UserServicePtr;

#endif
