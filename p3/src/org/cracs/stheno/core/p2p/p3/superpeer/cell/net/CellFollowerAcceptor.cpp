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
 * File:   CellFollowerAcceptor.cpp
 * Author: rmartins
 * 
 * Created on December 10, 2010, 12:08 PM
 */

#include "CellFollowerAcceptor.h"

#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellFollowerSvcHandler.h>

CellFollowerAcceptor::CellFollowerAcceptor(Cell* cell) :
m_cell(cell),
m_tpcPrt(0) {
}

CellFollowerAcceptor::~CellFollowerAcceptor() {
}

int CellFollowerAcceptor::open(void *arg) {
    throw -1;
}

int CellFollowerAcceptor::close() {
    m_acceptor.close();
    m_cell = 0;
    if (m_tpcPrt != 0) {
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int CellFollowerAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
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

ACE_HANDLE CellFollowerAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int CellFollowerAcceptor::handle_input(ACE_HANDLE fd) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellFollowerAcceptor::handle_input - New incoming connection\n")));
    CellFollowerSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    /*ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);*/
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            CellFollowerSvcHandler(m_cell, false, false, tpcPrt, 0, 0, 0), -1);
    auto_ptr<CellFollowerSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T) failed to accept\n")),
            -1);
    p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO CellFollowerAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    (tpc)->bind(client);
    (tpc)->open();
    if (client->open() == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FAILED TO OPEN FOLLOWER client!!\n")));
        delete client;
        return -1;
    }
    
    //if (client->asynchronous(true, true) == -1) {    
    //changed:
    if (client->asynchronous(true, false) == -1) {    
        //no close listener is active, close and delete it
        client->close();        
        delete client;
        return -1;
    }
    
    if (client->setCloseListener(this) == -1) {
        //channel is already closed, so close and delete it
        client->close();
        delete client;
        return -1;

    }
    
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: client binded OK!\n")));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int CellFollowerAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void CellFollowerAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    delete channel;
}

int CellFollowerAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);

}

EndpointPtr& CellFollowerAcceptor::getEndpoint() {
    return m_endpointPtr;
}

