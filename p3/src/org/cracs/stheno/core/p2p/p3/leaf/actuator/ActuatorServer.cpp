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

#include "ActuatorServer.h"

#include <euryale/net/NetworkUtils.h>
#include <euryale/net/endpoint/exception/EndpointException.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/QoSManagerInterface.h>

ActuatorServer::ActuatorServer() :
m_acceptor(this) {
}

ActuatorServer::~ActuatorServer() {
}

QoSManagerInterface* ActuatorServer::getQoSManager() {
    return m_qosManager;
}

int ActuatorServer::open(QoSManagerInterface* qosManager) {
    m_qosManager = qosManager;
    m_stop = false;    
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
			     //new QoSEndpoint(QoS::SRT, 25)));
        if (m_acceptor.open(endpoint) == 0) {
            bindOk = true;
            break;
        }
    }
    if (!bindOk) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorServer::open() - Acceptor() - bind nok\n")));
        throw EndpointException("Unable to bind");
    }

    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, 50);
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    return 0;
}

EndpointPtr& ActuatorServer::getEndpoint() {
    return m_acceptor.getEndpoint();
}


bool ActuatorServer::handleAction(ACE_Message_Block* payload){
    //printf("Received action!\n");
    ACE_Message_Block::release(payload);
    return true;
}
