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
 * File:   ActuatorServiceClient.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 8:26 PM
 */

#include "ActuatorServiceClient.h"
#include <stheno/services/actuator/ActuatorService.h>
#include <stheno/services/actuator/ActuatorServiceClient.h>
#include <stheno/services/actuator/net/ActuatorServiceClientHandler.h>
#include "stheno/Stheno.h"
#include <ace/SOCK_Connector.h>
#include <ace/Connector.h>

#include "stheno/core/p2p/OverlayInterface.h"
#include "stheno/core/p2p/discovery/general_provider/FindServiceInstanceQueryReply.h"
#include <euryale/common/sleep/SleepHelper.h>
#include <stheno/core/p2p/discovery/general_provider/FindServiceInstanceQuery.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <euryale/common/sleep/IncrementalSleep.h>

#include <stheno/core/QoSManager.h>

ActuatorServiceClient::ActuatorServiceClient(Stheno* runtime, ActuatorService* actuatorService) :
ServiceClient(runtime), m_actuatorService(actuatorService), m_opened(false), m_closed(false), m_rebinding(false),
m_rebindTotalTime(0, 0), m_rebindStart(0, 0), m_clientHandler(0) {
}

ActuatorServiceClient::~ActuatorServiceClient() {
}

int ActuatorServiceClient::open(const UUIDPtr& sid, const UUIDPtr& iid, ClientParamsPtr& cp) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (m_opened) {
        return -1;
    }
    m_opened = true;
    m_sid = sid;
    m_iid = iid;
    m_cp = cp;

    int ret = bindClientHandler();
    if (ret == -1) {
        m_opened = false;
        return -1;
    }
    /*if (m_clientHandler->setCloseListener(this) == -1) {
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
        return -1;
        }*/
    return 0;
}
//client shutdown/close

int ActuatorServiceClient::bindClientHandler() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    FindServiceInstanceQuery* query = new FindServiceInstanceQuery(m_sid, m_iid);
    Stheno* runtime = this->getRuntime();
    OverlayInterface* oi = runtime->getOverlayInterface();
    DiscoveryInterfacePtr discoveryPtr = oi->getDiscovery();
    printf("ActuatorServiceClient::bindClientHandler() OI=%p\n", oi);
    DiscoveryQueryReply* queryReply_ = discoveryPtr->executeQuery(query);
    if (queryReply_ == 0) {
        return -1;
    }
    if (queryReply_->isException()) {
        //printf("ActuatorServiceClient: exception! SID(%s) IID(%s)\n", m_sid->toString().c_str(), m_iid->toString().c_str());
        delete queryReply_;
        return -1;
    }
    FindServiceInstanceQueryReply *queryReply = new FindServiceInstanceQueryReply(queryReply_);

    //printf("StreamClient: FindServiceInstanceQueryReply!\n",queryReply->
    //EndpointPtr endpoint = m_streamParams->getEndpoint();
    Endpoint endpoint;
    //queryReply->getSAPInfo()->getFirstEndpoint(endpoint);
    queryReply->getSAPInfo()->getEndpoint(m_cp->getQoS(), endpoint);
    printf("ActuatorServiceClient: FindServiceInstanceQueryReply! endpoint=%s\n", endpoint.toString().c_str());
    ACE_Connector<ActuatorServiceClientHandler, ACE_SOCK_Connector> connector;
    //QoSEndpoint qosE = *(endpoint.getQoS());        

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    //QoSEndpoint qosEndpoint(Endpoint::TCP, 50);
    UUIDPtr runtimeUUID;
    UUIDPtr fid;
    m_runtime->getUUID(runtimeUUID);
    m_runtime->getOverlayInterface()->getMesh()->getFID(fid);

    m_clientHandler = new ActuatorServiceClientHandler(
            runtimeUUID,
            fid,
            false, false, tpcPrt, 0, 0, 0);

    printf("ActuatorServiceClient: before tpc\n");
    //tpc->open();
    CPUReservation* reserve = 0;
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
    CPUQoS* cpuQoS = m_cp->createCPUQoS(); //new CPUPriorityQoS(m_cp->getScheduleType(), m_cp->getPriority());
    tpc->bind(m_clientHandler);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(m_clientHandler);
    connector.reactor(tpc->getResources()->getReactor());
    printf("ActuatorServiceClient: before connect\n");
    if (connector.connect(m_clientHandler, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: ActuatorServiceClient::executeQuery() - connect failed:")));
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
        return -1;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorServiceClient::open_i() - Connect to service! %@\n"), m_clientHandler));
        if (m_rebindStart.msec() != 0) {
            ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - m_rebindStart;
            m_rebindTotalTime += rebindEnd;
        }
    }
    printf("ActuatorServiceClient: after connect\n");
    //m_client->setCloseListener(this);

    /*if (m_clientHandler->setCloseListener(this) == -1) {
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;        
        return -1;
    }*/


    //if (m_clientHandler->asynchronous(true, false) != 0) {
    if (m_clientHandler->asynchronous(true, false) != 0) {
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
        return -1;
    }
    return 0;
}

void ActuatorServiceClient::close() {
    /*if (m_closed) {
      return;
      }
      m_closed = true;*/
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_closed) {
        return;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("ActuatorServiceClient: close()\n")));
    m_closed = true;
    if (m_clientHandler != 0) {
        m_clientHandler->close(ACE_Event_Handler::DONT_CALL);
        delete m_clientHandler;
        m_clientHandler = 0;
    }
}

void ActuatorServiceClient::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_rebinding = true;
    printf("\n\n\n\n\n********************ActuatorServiceClient::onCloses %p\n", getRuntime()->getOverlayInterface());
    if (m_clientHandler != 0) {
        m_clientHandler->setCloseListener(0);
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
    }
    ace_mon.release();
    //SleepHelper::sleep(3000);
    //IncrementalSleep sleeper(0, 500000);
    //SleepHelper::sleep(250);
    while (!m_closed) {
        //printf("\n\n\n\n\n********************ActuatorServiceClient::onClose still binding 1 - m_client=%p\n", getRuntime()->getOverlayInterface());
        while (bindClientHandler() == -1) {
            //printf("\n\n\n\n\n********************ActuatorServiceClient::onClose still binding 2- m_client=%p\n", m_clientHandler);
            //SleepHelper::sleep(1000);
            //sleeper.sleep();
            //SleepHelper::sleep(250);
        }
        if (m_clientHandler->setCloseListener(this) == 0) {
            ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
            m_rebinding = false;
            break;
        } else {
            m_clientHandler->close();
            delete m_clientHandler;
            m_clientHandler = 0;
        }
    }
}

//has to have the same SID of the service it belongs

const UUIDPtr& ActuatorServiceClient::getSID() {
    return m_sid;
}

const UUIDPtr& ActuatorServiceClient::getIID() {
    return m_iid;
}

void ActuatorServiceClient::action(ACE_Message_Block* arg) throw (ActuatorException&) {
    //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    /*if (m_rebinding || m_clientHandler == 0) {
      ACE_Message_Block::release(arg);
      throw ActuatorException(ActuatorException::INVALID_RUNTIME);
      }*/
    if (m_clientHandler == 0) {
        if (this->bindClientHandler() == -1) {
            throw ActuatorException(ActuatorException::QOS_CLIENT_INITIALIZATION_ERROR);
        } else {
            m_rebindStart = ACE_Time_Value(0, 0);
        }
    }

    try {
        m_clientHandler->action(arg);

    } catch (ActuatorException& ex) {
        if (m_rebindStart.msec() == 0) {
            m_rebindStart = ACE_OS::gettimeofday();
        }
        if (m_clientHandler != 0) {
            delete m_clientHandler;
            m_clientHandler = 0;
        }
        throw ex;
    }
}

ACE_Time_Value& ActuatorServiceClient::getRebindTotalTime() {
    return this->m_rebindTotalTime;
}
