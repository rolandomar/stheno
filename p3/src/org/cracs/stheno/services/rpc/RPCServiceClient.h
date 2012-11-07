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
 * File:   RPCServiceClient.h
 * Author: rmartins
 *
 * Created on January 22, 2011, 8:26 PM
 */

#ifndef RPCSERVICECLIENT_H
#define	RPCSERVICECLIENT_H

#include <stheno/service/ServiceClient.h>
#include <stheno/services/rpc/exception/RPCException.h>
#include <euryale/net/AbstractStreamChannel.h>

class RPCService;
class RPCServiceClientHandler;
class RPCServiceClient: public ServiceClient,public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:    
    RPCServiceClient(Stheno* runtime,RPCService* rpcService);    
    virtual ~RPCServiceClient();
    
    virtual int open(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    //client shutdown/close
    virtual void close();
    //has to have the same SID of the service it belongs
    virtual const UUIDPtr& getSID();
    virtual const UUIDPtr& getIID();
    
    ACE_Message_Block* twoWayInvocation(UUIDPtr& oid,ACE_Message_Block* arg) throw(RPCException&);    
    void oneWayInvocation(UUIDPtr& oid,ACE_Message_Block* arg) throw(RPCException&);    
    
    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    
    virtual ACE_Time_Value& getRebindTotalTime();
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    RPCService* m_rpcService;
    RPCServiceClientHandler* m_clientHandler;
    ClientParamsPtr m_cp;
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    bool m_opened;
    bool m_closed;
    bool m_rebinding;
    ACE_Time_Value m_rebindTotalTime;
    ACE_Time_Value m_rebindStart;
    
    
    int bindClientHandler();

};

#endif	/* RPCSERVICECLIENT_H */

