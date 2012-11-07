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
 * File:   P3DiscoveryAcceptor.cpp
 * Author: rmartins
 * 
 * Created on September 17, 2010, 11:46 AM
 */

#include "P3DiscoveryAcceptor.h"
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/QoSManager.h>
#include <stheno/core/QoSManagerInterface.h>

/*P3DiscoveryAcceptor::P3DiscoveryAcceptor(P3Discovery* discovery,
                 StreamChannelGroup<ACE_SOCK_Stream, ACE_MT_SYNCH>* neighbours) :
m_discovery(discovery),
m_clientHandlers(neighbours){
}*/

P3DiscoveryAcceptor::P3DiscoveryAcceptor(P3Discovery* discovery) :
m_discovery(discovery),m_tpcPrt(0) {
}

P3DiscoveryAcceptor::~P3DiscoveryAcceptor() {
    close();
}

int P3DiscoveryAcceptor::close() {
    m_acceptor.close();
    m_discovery = 0;
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int P3DiscoveryAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (m_discovery->getQoSManager() != 0) {
        reserve = m_discovery->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(this);
    tpc->open(reserve, cpuQoS);
    //tpc->bind(this);   
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE P3DiscoveryAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int P3DiscoveryAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryAcceptor::handle_input - New incoming connection\n")));
    P3DiscoverySvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            P3DiscoverySvcHandler(m_discovery, tpcPrt, 0, 0, 0), -1);
    //auto_ptr<P3DiscoverySvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1){
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO P3DiscoveryAcceptor::handle_input - failed accept connection %@!\n"), client));
        client->close();
        delete client;
        return -1;
    }
    //p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO P3DiscoveryAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //m_cellGroup->add(client);
    //tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if(m_discovery->getQoSManager()!=0){
        reserve = m_discovery->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    tpc->bind(client);
    tpc->open(reserve, cpuQoS);    
    client->open();

    if(m_scvHandlers.add(client) == -1){
        return 0;
    }
    //if(client->asynchronous(true, true) == -1){
    //changed:
    if(client->asynchronous(true, false) == -1){
        client->close();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryAcceptor - client binded NOK! failed async\n")));
        return 0;
    }

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryAcceptor - client binded OK!\n")));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int P3DiscoveryAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void P3DiscoveryAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO: check this
    //m_scvHandlers.remove(channel);
}

int P3DiscoveryAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);

}

EndpointPtr& P3DiscoveryAcceptor::getEndpoint() {
    return m_endpointPtr;
}