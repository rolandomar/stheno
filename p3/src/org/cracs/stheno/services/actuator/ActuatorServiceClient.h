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
 * File:   ActuatorServiceClient.h
 * Author: rmartins
 *
 * Created on January 22, 2011, 8:26 PM
 */

#ifndef ActuatorServiceCLIENT_H
#define	ActuatorServiceCLIENT_H

#include <stheno/service/ServiceClient.h>
#include <stheno/services/actuator/exception/ActuatorException.h>
#include <euryale/net/AbstractStreamChannel.h>

class ActuatorService;
class ActuatorServiceClientHandler;
class ActuatorServiceClient: public ServiceClient,public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:    
    ActuatorServiceClient(Stheno* runtime,ActuatorService* actuatorService);    
    virtual ~ActuatorServiceClient();
    
    virtual int open(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    //client shutdown/close
    virtual void close();
    //has to have the same SID of the service it belongs
    virtual const UUIDPtr& getSID();
    virtual const UUIDPtr& getIID();
        
    void action(ACE_Message_Block* arg) throw(ActuatorException&);    
    
    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    ACE_Time_Value& getRebindTotalTime();
        
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    ActuatorService* m_actuatorService;
    ActuatorServiceClientHandler* m_clientHandler;
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    ClientParamsPtr m_cp;
    bool m_opened;
    bool m_closed;
    bool m_rebinding;
    ACE_Time_Value m_rebindTotalTime;
    ACE_Time_Value m_rebindStart;
    
    int bindClientHandler();

};

#endif	/* ActuatorServiceCLIENT_H */

