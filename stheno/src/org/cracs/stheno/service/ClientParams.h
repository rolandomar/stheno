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
 * File:   ClientParams.h
 * Author: rmartins
 *
 * Created on February 11, 2011, 3:28 PM
 */

#ifndef CLIENTPARAMS_H
#define	CLIENTPARAMS_H

#include <ace/Synch.h>
#include <ace/Refcounted_Auto_Ptr.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/QoS.h>
#include <euryale/net/endpoint/Endpoint.h>

class ClientParams {
public:
    ClientParams(int qos,int priority);
    ClientParams(const ClientParams& orig);
    virtual ~ClientParams();
    
    //int getQoS();
    int getScheduleType() {
        return m_qos.getScheduleType();
    }
    
    CPUQoS* createCPUQoS(){
        return new CPUPriorityQoS(m_qos.getScheduleType(), m_qos.getPriority());
    }
    
    //int getPriority();
    QoSEndpoint& getQoS();
    
protected:
    QoSEndpoint m_qos;
    //int m_qos;
    //int m_priority;
};

typedef ACE_Strong_Bound_Ptr<ClientParams, ACE_SYNCH_MUTEX> ClientParamsPtr;

#endif	/* CLIENTPARAMS_H */

