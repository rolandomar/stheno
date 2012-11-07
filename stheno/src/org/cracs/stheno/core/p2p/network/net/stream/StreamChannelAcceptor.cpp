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
 * File:   StreamChannelAcceptor.cpp
 * Author: rmartins
 * 
 * Created on October 27, 2010, 10:42 AM
 */

#include "StreamChannelAcceptor.h"
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/network/net/stream/StreamChannelSvcHandler.h>

StreamChannelAcceptor::StreamChannelAcceptor(IncomingStreamClientHandler* clientHandler) :
m_clientHandler(clientHandler), m_scvHandlers(this),m_tpcPrt(0)
/*m_discovery(discovery)*/ {
}

StreamChannelAcceptor::~StreamChannelAcceptor() {
}

int StreamChannelAcceptor::close() {
    m_acceptor.close();
    if(m_tpcPrt!=0){
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    //m_discovery = 0;
    return 0;
}

int StreamChannelAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
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

ACE_HANDLE StreamChannelAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

ACE_SOCK_Acceptor& StreamChannelAcceptor::getAcceptorSock() {
    return m_acceptor;
}

// Called when a connection is ready to accept.

int StreamChannelAcceptor::handle_input(ACE_HANDLE fd) {
    return m_clientHandler->handleNewClient(this);
}

/*void StreamChannelAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {    
    m_scvHandlers.remove(channel);    
    m_clientHandler->handleCloseClient(this,static_cast<StreamChannel*>(channel));
}*/

// Called when this handler is removed from the ACE_Reactor.

int StreamChannelAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

int StreamChannelAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);

}

EndpointPtr& StreamChannelAcceptor::getEndpoint() {
    return m_endpointPtr;
}

void StreamChannelAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    m_clientHandler->handleCloseClient(this, static_cast<StreamChannel*> (channel));
}

void StreamChannelAcceptor::onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
}