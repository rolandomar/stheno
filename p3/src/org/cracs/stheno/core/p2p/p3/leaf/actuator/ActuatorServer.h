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
 * File:   StreamService.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:45 PM
 */

#ifndef ActuatorServer_H
#define	ActuatorServer_H

#include <euryale/threading/Task.h>
#include <euryale/common/sleep/SleepHelper.h>

#include <stheno/services/actuator/ServiceActuatorParams.h>
#include <stheno/core/p2p/p3/leaf/actuator/net/ActuatorSvcHandler.h>
#include <stheno/core/p2p/p3/leaf/actuator/net/ActuatorAcceptor.h>


class QoSManagerInterface;
class ActuatorServer {
public:
    ActuatorServer();
    virtual ~ActuatorServer();

    int open(QoSManagerInterface* qosManager);

    EndpointPtr& getEndpoint();  
    
    QoSManagerInterface* getQoSManager();
    
    bool handleAction(ACE_Message_Block* payload);
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;       
    ActuatorAcceptor m_acceptor;
    bool m_stop;
    QoSManagerInterface* m_qosManager;
};

#endif	/* ActuatorServer_H */

