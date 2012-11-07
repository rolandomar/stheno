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
 * File:   StreamService.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:45 PM
 */

#include "StreamServer.h"

#include <euryale/net/NetworkUtils.h>
#include <euryale/net/endpoint/exception/EndpointException.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/p2p/p3/leaf/stream/StreamSession.h>
#include <stheno/core/QoSManagerInterface.h>

StreamServer::StreamServer(int type) :
m_acceptor(this), m_type(type) {
}

StreamServer::~StreamServer() {
}

int StreamServer::createSession(StreamSvcHandler* svcHandler) {
    StreamSession* session = new StreamSession(this, svcHandler);
    int ret = session->open();
    return ret;
}

QoSManagerInterface* StreamServer::getQoSManager() {
    return m_qosManager;
}

int StreamServer::open(QoSManagerInterface* qosManager) {
    m_qosManager = qosManager;
    m_stop = false;
    m_frameCounter = 0;
    int maxBinds = 10000;
    bool bindOk = false;
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    ACE_INET_Addr* addr = NetworkUtils::get_ip_interface(itf.c_str());
    if (addr == 0) {
        throw EndpointException("Invalid interface");
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - interface: %s IP=%s\n"), itf.c_str(), addr->get_host_addr()));
    String ip = addr->get_host_addr();
    delete addr;
    for (int i = 0; i < maxBinds; i++) {
        UShort port = NetworkUtils::randomPort();
        EndpointPtr endpoint(
                new Endpoint(Endpoint::TCP, ip,
                port, new QoSEndpoint(QoS::HRT, CPUQoS::MAX_RT_PRIO)));
        if (m_acceptor.open(endpoint) == 0) {
            bindOk = true;
            break;
        }
    }
    if (!bindOk) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServer::open() - Acceptor() - bind nok\n")));
        throw EndpointException("Unable to bind");
        //throw ServiceException(ServiceException::INSUFFICIENT_RESOURCES);
    }

    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    return Task::activate(reserve, cpuQoS);

    /*CPUPriorityRTFifo* cpuQoS = new CPUPriorityRTFifo(50);
    return Task::activate(0, cpuQoS);*/
}

EndpointPtr& StreamServer::getEndpoint() {
    return m_acceptor.getEndpoint();
}

int StreamServer::svc_i(void*) {
    float sleepTime = ((float) 1 / (float) getLowLevelFrameRate())*1000000;
    while (!m_stop) {
        m_frameCounter++;
        fireStreamListener();
        SleepHelper::sleep(0, sleepTime);
        //SleepHelper::sleep(2, 0);
    }
    return 0;
}

UInt StreamServer::getLowLevelFrameRate() {
    return 24;
}

void StreamServer::addStreamListener(StreamListener* sl) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_listeners.push_back(sl);
}

void StreamServer::fireStreamListener() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    list<StreamListener*>::iterator iter = m_listeners.begin();
    while (iter != m_listeners.end()) {
        StreamListener* sl = *iter;
        if (sl->onFrame(m_frameCounter) == -1) {
            iter = m_listeners.erase(iter);
            delete sl;
        } else {
            //printf("Sent frame\n");
            iter++;
        }
    }
} 