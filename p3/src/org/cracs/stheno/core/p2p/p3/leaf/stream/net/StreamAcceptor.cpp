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
 * File:   StreamAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 22, 2010, 12:28 PM
 */

#include "StreamAcceptor.h"
#include <stheno/core/p2p/p3/leaf/stream/net/StreamSvcHandler.h>
#include <stheno/core/p2p/p3/leaf/stream/StreamServer.h>
#include <stheno/core/QoSManager.h>
#include <stheno/core/QoSManagerInterface.h>

StreamAcceptor::StreamAcceptor(StreamServer* discovery) :
m_streamServer(discovery),m_tpcPrt(0) {
}

StreamAcceptor::~StreamAcceptor() {
}

int StreamAcceptor::close() {
    m_acceptor.close();
    m_streamServer = 0;
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int StreamAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT,CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if(m_streamServer->getQoSManager()!=0){
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT",cpuQoS);
    }
    tpc->bind(this);
    tpc->open(reserve,cpuQoS);
    //tpc->bind(this);
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE StreamAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int StreamAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamAcceptor::handle_input - New incoming connection\n")));
    StreamSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            StreamSvcHandler(m_streamServer, tpcPrt, 0, 0, 0), -1);
    auto_ptr<StreamSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: StreamAcceptor() -  failed to accept\n")),
            -1);
    p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO StreamAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //m_cellGroup->add(client);
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT,CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if(m_streamServer->getQoSManager()!=0){
        reserve = m_streamServer->getQoSManager()->createCPUReservation("HRT",cpuQoS);
    }
    tpc->bind(client);
    tpc->open(reserve,cpuQoS);
    //tpc->bind(client);
    if(client->open() != 0){
        client->close();
        delete client;
    }
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int StreamAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void StreamAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //m_scvHandlers.remove(channel);
}

int StreamAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& StreamAcceptor::getEndpoint() {
    return m_endpointPtr;
}
