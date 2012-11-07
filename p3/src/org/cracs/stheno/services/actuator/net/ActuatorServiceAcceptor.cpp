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
 * File:   ActuatorServiceAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 24, 2010, 11:52 AM
 */

#include "ActuatorServiceAcceptor.h"

#include <stheno/services/actuator/net/ActuatorServiceSvcHandler.h>
#include <stheno/services/actuator/ActuatorService.h>

ActuatorServiceAcceptor::ActuatorServiceAcceptor(ActuatorService* discovery) :
m_streamServer(discovery) {
}

ActuatorServiceAcceptor::~ActuatorServiceAcceptor() {
}

int ActuatorServiceAcceptor::close() {
    m_acceptor.close();
    m_streamServer = 0;
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int ActuatorServiceAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorServiceAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    (*m_tpcPrt)->bind(this);
    (*m_tpcPrt)->open();    
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE ActuatorServiceAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int ActuatorServiceAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorServiceAcceptor::handle_input - New incoming connection\n")));
    ActuatorServiceSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //     ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            ActuatorServiceSvcHandler(m_streamServer, tpcPrt, 0, 0, 0), -1);
    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: ActuatorServiceAcceptor() -  failed to accept\n")),
            -1);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO ActuatorServiceAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    
    CPUReservation* reserve = 0;    
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    if(m_streamServer->getQoSManager()!=0){
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT",cpuQoS);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO ActuatorServiceAcceptor::handle_input - new connection %d with QOS!\n"), client->get_handle()));
    }else{
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO ActuatorServiceAcceptor::handle_input - new connection %d without QOS!\n"), client->get_handle()));
    }
    (tpc)->bind(client);
    (tpc)->open(reserve,cpuQoS);
    client->open();
        
    if(m_scvHandlers.add(client) == -1){
        return 0;
    }
    if(client->asynchronous(true, false) == -1){
        client->close();        
    }
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int ActuatorServiceAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void ActuatorServiceAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO:check this
    m_scvHandlers.remove(channel);
}

int ActuatorServiceAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& ActuatorServiceAcceptor::getEndpoint() {
    return m_endpointPtr;
}
