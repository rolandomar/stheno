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
 * File:   RPCServiceAcceptor.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:06 PM
 */

#include "RPCServiceAcceptor.h"

#include <stheno/services/rpc/net/RPCServiceSvcHandler.h>
#include <stheno/services/rpc/RPCService.h>

#include <stheno/core/QoSManager.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

RPCServiceAcceptor::RPCServiceAcceptor(RPCService* discovery) :
m_streamServer(discovery), m_tpcPrt(0) {
}

RPCServiceAcceptor::~RPCServiceAcceptor() {
}

int RPCServiceAcceptor::close() {
    m_acceptor.close();
    m_streamServer = 0;
    if (m_tpcPrt != 0) {
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int RPCServiceAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamServiceAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    //tpc->bind(this);
    //tpc->open();    
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    if (m_streamServer->getQoSManager() != 0) {     
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }    
    tpc->bind(this);
    tpc->open(reserve, cpuQoS);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: RPCServiceAcceptor::open - reactor(%@) tpc_Reactor=%@\n"),this->reactor(),tpc->getResources()->getReactor()));
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE RPCServiceAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int RPCServiceAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceAcceptor::handle_input - New incoming connection\n")));
    RPCServiceSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);

    ACE_NEW_RETURN(client,
            RPCServiceSvcHandler(m_streamServer, tpcPrt, 0, 0, 0), -1);
    auto_ptr<RPCServiceSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: StreamServiceAcceptor() -  failed to accept\n")),
            -1);
    p.release();
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO RPCServiceAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //m_cellGroup->add(client);
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    if (m_streamServer->getQoSManager() != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO RPCServiceAcceptor::with QOS type=%d\n"),m_streamServer->getType()));
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }else{
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO RPCServiceAcceptor::with No QOS type=%d\n"),m_streamServer->getType()));
    }
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);    
    tpc->bind(client);
    tpc->open(reserve, cpuQoS);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO RPCServiceAcceptor::handle_input - new connection %d! reactor=%@ tpcreactor=%@\n"), 
            client->get_handle(),client->reactor(),tpc->getResources()->getReactor()));
    //tpc->bind(client);
    client->open();
    //m_streamServer->createSession(client);
    if (m_scvHandlers.add(client) == -1) {
        return 0;
    }
    if (client->asynchronous(true, false) == -1) {
        client->close();
    }

    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceAcceptor - client binded OK! ret=%d\n"),ret));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int RPCServiceAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void RPCServiceAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO: check this
    //m_scvHandlers.remove(channel);   
}

int RPCServiceAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& RPCServiceAcceptor::getEndpoint() {
    return m_endpointPtr;
}
