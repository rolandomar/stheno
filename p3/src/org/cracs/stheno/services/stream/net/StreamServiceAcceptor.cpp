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
 * File:   StreamServiceAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 24, 2010, 11:52 AM
 */

#include "StreamServiceAcceptor.h"

#include <stheno/services/stream/net/StreamServiceSvcHandler.h>
#include <stheno/services/stream/StreamService.h>

StreamServiceAcceptor::StreamServiceAcceptor(StreamService* discovery) :
m_streamServer(discovery),m_tpcPrt(0) {
}

StreamServiceAcceptor::~StreamServiceAcceptor() {
}

int StreamServiceAcceptor::close() {
    m_acceptor.close();
    m_streamServer = 0;
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int StreamServiceAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamServiceAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    tpc->bind(this);
    tpc->open();
    //tpc->bind(this);
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE StreamServiceAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int StreamServiceAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceAcceptor::handle_input - New incoming connection\n")));
    StreamServiceSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            StreamServiceSvcHandler(m_streamServer, tpcPrt, 0, 0, 0), -1);
    auto_ptr<StreamServiceSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: StreamServiceAcceptor() -  failed to accept\n")),
            -1);
    p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO StreamServiceAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //m_cellGroup->add(client);
    tpc->bind(client);
    tpc->open();
    //tpc->bind(client);
    if(client->open() == -1){
        client->close();
        delete client;
        return 0;
    }       
    //m_streamServer->createSession(client);
    if(m_scvHandlers.add(client) == -1){
        return 0;
    }
    //int ret = client->asynchronous(true, true);    
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceAcceptor - client binded OK! ret=%d\n"),ret));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int StreamServiceAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void StreamServiceAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO: check this
    //m_scvHandlers.remove(channel);
}

int StreamServiceAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& StreamServiceAcceptor::getEndpoint() {
    return m_endpointPtr;
}
