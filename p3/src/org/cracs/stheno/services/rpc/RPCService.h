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
 * File:   RPCService.h
 * Author: rmartins
 *
 * Created on January 21, 2011, 2:25 PM
 */

#ifndef RPCSERVICE_H
#define	RPCSERVICE_H


#include <stheno/service/UserService.h>
#include <stheno/core/p2p/OverlayInterface.h>

#include <stheno/services/rpc/RPCInterface.h>
#include <stheno/services/rpc/RPCTestObject.h>
#include <stheno/services/rpc/RPCRuntimeManagerObject.h>
#include <stheno/services/rpc/net/RPCServiceSAP.h>

class RPCServiceClient;
class RPCService : public UserService/*,RPCInterface*/{
public:
    RPCService();
    RPCService(Stheno* runtime);
    RPCService(Stheno* runtime,UUIDPtr& iid);    
    virtual ~RPCService();
    
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    virtual void close();
    virtual ServiceClient* getClient(const UUIDPtr& sid,ClientParamsPtr& cp);
    virtual ServiceClient* getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    
    
    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);
    virtual ServiceAbstract* duplicate() throw (ServiceException&);
    virtual const char* getName();    
    virtual void getSAP(SAPInfoPtr& sap);    
    virtual const char* id();
    
    //Server side
    void handleOneWayInvocation(UUIDPtr& oid,ACE_Message_Block* arg);
    ACE_Message_Block* handleTwoWayInvocation(UUIDPtr& oid,ACE_Message_Block* arg);
    
    
    void onReplication(ACE_Message_Block* replicationData) throw (ServiceException&);   
            
protected:
    RPCTestObject m_testObject;
    RPCRuntimeManagerObject m_runtimeManagerObject;
    RPCServiceSAP* m_sap;

};

#endif	/* RPCSERVICE_H */

