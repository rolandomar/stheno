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
 * File:   RPCServiceClient.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 8:26 PM
 */

#include "RPCServiceClient.h"
#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCServiceClient.h>
#include <stheno/services/rpc/net/RPCServiceClientHandler.h>
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

RPCServiceClient::RPCServiceClient(Stheno* runtime, RPCService* rpcService) :
ServiceClient(runtime), m_rpcService(rpcService), m_opened(false), m_closed(false), m_rebinding(false),
m_rebindTotalTime(0, 0), m_rebindStart(0, 0), m_clientHandler(0) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceClient:RPCServiceClient(1) %@\n"), this));
}

RPCServiceClient::~RPCServiceClient() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceClient::~RPCServiceClient() %@\n"), this));
    close();
}

int RPCServiceClient::open(const UUIDPtr& sid, const UUIDPtr& iid, ClientParamsPtr& cp) {
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

int RPCServiceClient::bindClientHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler()\n")));
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    FindServiceInstanceQuery* query = new FindServiceInstanceQuery(m_sid, m_iid);
    Stheno* runtime = this->getRuntime();
    OverlayInterface* oi = runtime->getOverlayInterface();
    DiscoveryInterfacePtr discoveryPtr = oi->getDiscovery();
    //printf("RPCServiceClient::bindClientHandler() OI=%p\n", oi);
    DiscoveryQueryReply* queryReply_ = discoveryPtr->executeQuery(query);
    //query is already owned
    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("********************RPCServiceClient::bindClientHandler %s %s %p\n"),
            m_sid->toString().c_str(),
            m_iid->toString().c_str(),
            queryReply_));*/
    if (queryReply_ == 0) {
        printf("query == bnull!\n");
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler() query null!\n")));
        return -1;
    }
    if (queryReply_->isException()) {
        printf("query == ex!\n");
        delete queryReply_;
        return -1;
    }
    FindServiceInstanceQueryReply *queryReply = new FindServiceInstanceQueryReply(queryReply_);

    Endpoint endpoint;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler() before endpoint choosing\n")));
    try {
        queryReply->getSAPInfo()->getEndpoint(m_cp->getQoS(), endpoint);
    } catch (EndpointException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler() endpoint EX!\n")));
        delete queryReply;
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: bindClientHandler! endpoint=%s\n"), endpoint.toString().c_str()));
    ACE_Connector<RPCServiceClientHandler, ACE_SOCK_Connector> connector;
    //QoSEndpoint qosE = *(endpoint.getQoS());        

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    //QoSEndpoint qosEndpoint(Endpoint::TCP, 50);
    if (m_clientHandler != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler() closing open client\n")));
        m_clientHandler->close();
        delete m_clientHandler;
    }

    UUIDPtr runtimeUUID;
    UUIDPtr fid;
    m_runtime->getUUID(runtimeUUID);
    m_runtime->getOverlayInterface()->getMesh()->getFID(fid);
    m_clientHandler = new RPCServiceClientHandler(
            runtimeUUID,
            fid,
            m_cp->getQoS(),
            false, false, tpcPrt, 0, 0, 0);

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: RPCServiceClient::bindClientHandler() endpoint client=%@\n"), m_clientHandler));
    //tpc->open();
    CPUReservation* reserve = 0;
    CPUQoS* cpuQoS = m_cp->createCPUQoS();
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
    //CPUQoS* cpuQoS = new CPUPriorityQoS(m_cp->getScheduleType(), m_cp->getPriority());    
    tpc->bind(m_clientHandler);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(m_clientHandler);
    connector.reactor(tpc->getResources()->getReactor());
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: bindClientHandler before connect\n")));
    if (connector.connect(m_clientHandler, endpoint.getAddr()) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)(%t|%T) ERROR: RPCServiceClient::bindClientHandler() - connect failed!")));
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
        return -1;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceClient::bindClientHandler() - Connect to service! client=%@\n"), m_clientHandler));
        if (m_rebindStart.msec() != 0) {
            ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - m_rebindStart;
            m_rebindTotalTime += rebindEnd;
            m_rebindStart = ACE_Time_Value(0,0);
        }
    }



    /*if (m_clientHandler->asynchronous(true, false) != 0) {
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
        return -1;
    }*/

    /*if (m_rebindStart.msec() != 0) {
        ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - m_rebindStart;
        m_rebindTotalTime += rebindEnd;
    }*/

    return 0;
}

void RPCServiceClient::close() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_closed) {
        return;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) RPCServiceClient: close()\n")));
    m_closed = true;
    if (m_clientHandler != 0) {
        m_clientHandler->close(ACE_Event_Handler::DONT_CALL);
        delete m_clientHandler;
        m_clientHandler = 0;
    }
}

void RPCServiceClient::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:********************RPCServiceClient::onClose before lock channel=%@\n"),
            channel));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_closed || m_rebinding) {
        return;
    }
    m_rebinding = true;
    ACE_Time_Value rebindStart = ACE_OS::gettimeofday();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:********************RPCServiceClient::onClose %@ m_clientHandler=%@\n"),
            channel, m_clientHandler));
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
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:********************RPCServiceClient::onClose still binding 1 - m_client=%@\n"),
                m_clientHandler));
        while (bindClientHandler() == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:********************RPCServiceClient::onClose still binding 2- m_client=%@\n"),
                    m_clientHandler));
            //SleepHelper::sleep(1000);
            //sleeper.sleep();
            //SleepHelper::sleep(250);
        }
        {
            ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
            if (m_clientHandler->setCloseListener(this) == 0) {
                //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
                m_rebinding = false;
                //ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - rebindStart;
                //m_rebindTotalTime += rebindEnd;
                break;
            } else {
                m_clientHandler->close();
                delete m_clientHandler;
                m_clientHandler = 0;
            }
        }
    }
    //ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - rebindStart;
    //m_rebindTotalTime += rebindEnd;
}

//has to have the same SID of the service it belongs

const UUIDPtr& RPCServiceClient::getSID() {
    return m_sid;
}

const UUIDPtr& RPCServiceClient::getIID() {
    return m_iid;
}

ACE_Message_Block* RPCServiceClient::twoWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) throw (RPCException&) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    /*if (m_rebinding || m_clientHandler == 0) {
        ACE_Message_Block::release(arg);
        throw RPCException(RPCException::INVALID_RUNTIME);
    }*/
    if (m_clientHandler == 0) {
        if (this->bindClientHandler() == -1) {
            throw RPCException(RPCException::QOS_CLIENT_INITIALIZATION_ERROR);
        } else {
            m_rebindStart = ACE_Time_Value(0, 0);
        }
    }
    ACE_Message_Block* ret = 0;
    try {
        ret = m_clientHandler->twoWayInvocation(oid, arg);
    } catch (RPCException& ex) {
        if (m_rebindStart.msec() == 0) {
            m_rebindStart = ACE_OS::gettimeofday();
        }
        if (m_clientHandler != 0) {
            delete m_clientHandler;
            m_clientHandler = 0;
        }
        throw ex;
    }
    return ret;
}

void RPCServiceClient::oneWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) throw (RPCException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    /*if (m_rebinding || m_clientHandler == 0) {
        ACE_Message_Block::release(arg);
        throw RPCException(RPCException::INVALID_RUNTIME);
    }*/
    if (m_clientHandler == 0) {
        if (this->bindClientHandler() == -1) {
            throw RPCException(RPCException::QOS_CLIENT_INITIALIZATION_ERROR);
        } else {
            m_rebindStart = ACE_Time_Value(0, 0);
        }
    }
    try {
        m_clientHandler->oneWayInvocation(oid, arg);
    } catch (RPCException& ex) {
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

ACE_Time_Value& RPCServiceClient::getRebindTotalTime() {
    return this->m_rebindTotalTime;
}