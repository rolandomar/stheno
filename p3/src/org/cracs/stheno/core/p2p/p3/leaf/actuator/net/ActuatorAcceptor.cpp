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
 * File:   ActuatorAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 22, 2010, 12:28 PM
 */

#include "ActuatorAcceptor.h"
#include <stheno/core/p2p/p3/leaf/actuator/net/ActuatorSvcHandler.h>
#include <stheno/core/p2p/p3/leaf/actuator/ActuatorServer.h>
#include <stheno/core/QoSManager.h>
#include <stheno/core/QoSManagerInterface.h>

ActuatorAcceptor::ActuatorAcceptor(ActuatorServer* discovery) :
m_streamServer(discovery) {
}

ActuatorAcceptor::~ActuatorAcceptor() {
}

int ActuatorAcceptor::close() {
    m_acceptor.close();
    m_streamServer = 0;
    return 0;
}

int ActuatorAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new     
          //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    m_tpcPtr = new     
          ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO,50);
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT,CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if(m_streamServer->getQoSManager()!=0){
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT",cpuQoS);
    }
    (*m_tpcPtr)->bind(this);
    (*m_tpcPtr)->open(reserve,cpuQoS);
    //tpc->bind(this);
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE ActuatorAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int ActuatorAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorAcceptor::handle_input - New incoming connection\n")));
    ActuatorSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            ActuatorSvcHandler(m_streamServer, tpcPrt, 0, 0, 0), -1);    

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: ActuatorAcceptor() -  failed to accept\n")),
            -1);   
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO ActuatorAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO,50);
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT,CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if(m_streamServer->getQoSManager()!=0){
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT",cpuQoS);
    }
    (tpc)->bind(client);
    (tpc)->open(reserve,cpuQoS);
    
    if(client->open() != 0){
        client->close();
        delete client;
    }
    int ret = client->asynchronous(true, false);
    return ret;
}

// Called when this handler is removed from the ACE_Reactor.

int ActuatorAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void ActuatorAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //m_scvHandlers.remove(channel);
}

int ActuatorAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& ActuatorAcceptor::getEndpoint() {
    return m_endpointPtr;
}
