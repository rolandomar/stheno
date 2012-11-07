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
 * File:   FTDataAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:32 AM
 */

#include "FTDataAcceptor.h"
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataSvcHandler.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>
#include <stheno/core/QoSManagerInterface.h>
#include <stheno/core/QoSManager.h>

FTDataAcceptor::FTDataAcceptor(P3ReplicationGroup* ft) :
m_ft(ft),m_scvHandlers(ft) {
}

FTDataAcceptor::~FTDataAcceptor() {
}

int FTDataAcceptor::close() {
    m_acceptor.close();
    m_ft = 0;
    return 0;
}

int FTDataAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    m_tpcPrt.reset(tpc);
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    if (m_ft->getQoSManager() != 0) {
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(this);
    tpc->open(reserve,cpuQoS);
    //tpc->bind(this);    
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE FTDataAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int FTDataAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataAcceptor::handle_input - New incoming connection\n")));
    FTDataSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            FTDataSvcHandler(m_ft, tpcPrt, 0, 0, 0), -1);
    auto_ptr<FTDataSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: FTDataAcceptor() -  failed to accept\n")),
            -1);
    p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTDataAcceptor::handle_input - new connection %d!\n"), client->get_handle()));    
    //tpc->open();
    //TODO: changed this
    //CPUReservation* reserve = 0;
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    if (m_ft->getQoSManager() != 0) {
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(client);
    tpc->open(reserve,cpuQoS);
    //tpc->bind(client);
    if(client->open() == -1){
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTDataAcceptor::handle_input - open() =-1, client closed!\n")));
        client->close();
        delete client;
        return 0;
    }
        
    //m_streamServer->createSession(client);
    if(m_scvHandlers.add(client) == -1){
        return 0;
    }
    //int ret = client->asynchronous(true, true);
    if(client->asynchronous(true, false) == -1){
        client->close();
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataAcceptor - client binded OK! ret=%d\n"),ret));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int FTDataAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void FTDataAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO: check this
    //m_scvHandlers.remove(channel);
}

int FTDataAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& FTDataAcceptor::getEndpoint() {
    return m_endpointPtr;
}
