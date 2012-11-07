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
 * File:   LeafAcceptor.cpp
 * Author: rmartins
 * 
 * Created on October 11, 2010, 1:56 PM
 */

#include "LeafAcceptor.h"

#include <stheno/core/p2p/p3/leaf/mesh/P3LeafMesh.h>

/*P3DiscoveryAcceptor::P3DiscoveryAcceptor(P3Discovery* discovery,
                 StreamChannelGroup<ACE_SOCK_Stream, ACE_MT_SYNCH>* neighbours) :
m_discovery(discovery),
m_clientHandlers(neighbours){
}*/

LeafAcceptor::LeafAcceptor(P3LeafMesh* mesh) :
m_mesh(mesh),
m_tpcPrt(0){
}

LeafAcceptor::~LeafAcceptor() {
    close();
}

int LeafAcceptor::close() {
    m_acceptor.close();
    m_mesh = 0;
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int LeafAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    tpc->open();
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO,CPUQoS::MAX_RT_PRIO);
    //CPUReservation* reserve = m_mesh->getQoSManager()->createCPUReservation("HRT",cpuQoS);
    //tpc->open(reserve,cpuQoS);
    //tpc->bind(this);
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE LeafAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int LeafAcceptor::handle_input(ACE_HANDLE fd) {
    /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellAcceptor::handle_input - New incoming connection\n")));
    CellSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);

    ACE_NEW_RETURN(client,
                   CellSvcHandler(m_cell, (ExecutionModelPtr*) tpcPrt, 0, 0, 0), -1);
    auto_ptr<CellSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
                         ACE_TEXT("(%T) failed to accept\n")),
                         -1);
    p.release();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO CellAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //m_cellGroup->add(client);
    tpc->open();
    tpc->bind(client);
    client->open();
    if(client->isChildren()){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellAcceptor::handle_input() - Add new children peer (%s,%s)\n"),
                m_cell->getCellID()->toString().c_str(),client->getCellID()->toString().c_str()));
        m_childrenGroup->add(client);
        client->setCloseListener(m_childrenGroup);
    }else{
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellAcceptor::handle_input() - Add group peer\n")));
        m_cellGroup->add(client);
        client->setCloseListener(m_cellGroup);
    }
    client->asynchronous(true,true);

    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: client binded OK!\n")));*/
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int LeafAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void LeafAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) { 
    //TODO: check this:
    //m_scvHandlers.remove(channel);    
}

int LeafAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);

}

EndpointPtr& LeafAcceptor::getEndpoint() {
    return m_endpointPtr;
}
