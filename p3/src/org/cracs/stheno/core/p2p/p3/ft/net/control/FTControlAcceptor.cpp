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
 * File:   FTControlAcceptor.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:32 AM
 */

#include "FTControlAcceptor.h"
#include "stheno/core/p2p/p3/ft/P3FaultTolerance.h"
#include <stheno/core/QoSManagerInterface.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlSvcHandler.h>
//#include <stheno/core/p2p/ft/ReplicationGroup.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

FTControlAcceptor::FTControlAcceptor(P3FaultTolerance* ft) :
m_ft(ft),
m_tpcPrt(0) {
}

FTControlAcceptor::~FTControlAcceptor() {
}

int FTControlAcceptor::close() {
    m_acceptor.close();
    m_ft = 0;
    if (m_tpcPrt != 0) {
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int FTControlAcceptor::open(EndpointPtr& endpointPtr) throw (ExecutionModelException&) {
    m_endpointPtr = endpointPtr;
    if (this->m_acceptor.open(endpointPtr->getAddr(), 1) == -1) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlAcceptor::open - failed open\n")));
        return -1;
    }

    ThreadPerConnection *tpc = new ThreadPerConnection();
    m_tpcPrt = new
            ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - before QoS %d!\n"), client->get_handle()));
    if (m_ft->getQoSManager() != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::open - activating acceptor with QoS!\n")));
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }else{
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::open activating acceptor without QoS!\n")));
    }
    (*m_tpcPrt)->bind(this);
    (*m_tpcPrt)->open(reserve,cpuQoS);
    return this->reactor()->register_handler
            (this, ACE_Event_Handler::ACCEPT_MASK);
}


// Get this handler's I/O handle.

ACE_HANDLE FTControlAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int FTControlAcceptor::handle_input(ACE_HANDLE fd) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlAcceptor::handle_input - New incoming connection\n")));
    FTControlSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);

    ACE_NEW_RETURN(client,
            FTControlSvcHandler(m_ft, tpcPrt, 0, 0, 0), -1);
    auto_ptr<FTControlSvcHandler> p(client);

    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T)ERROR: FTControlAcceptor() -  failed to accept\n")),
            -1);
    p.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    //TODO: added this
    CPUQoS* cpuQoS = new CPUPriorityQoS(this->m_endpointPtr->getQoS()->getScheduleType(), m_endpointPtr->getQoS()->getPriority());
    CPUReservation* reserve = 0;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - before QoS %d!\n"), client->get_handle()));
    if (m_ft->getQoSManager() != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - activating QoS %d!\n"), client->get_handle()));
        reserve = m_ft->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }else{
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - activating without QoS %d!\n"), client->get_handle()));
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - after QoS!\n")));
    (tpc)->bind(client);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - after bind!\n")));
    (tpc)->open(reserve,cpuQoS);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - after tpc open!\n")));
    if (client->open() == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - client with open() == -1, closed!\n")));
        client->close();
        delete client;
        return 0;
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - after open!\n")));
    ReplicationGroupPtr rgPtr;
    m_ft->getReplicationGroup(client->getRGID(), rgPtr);
    P3ReplicationGroup* rg = static_cast<P3ReplicationGroup*> (rgPtr.get());
    if (rg->getControlChannelManager().add(client) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - add to group -1, closed!\n")));
        return -1;
    }    
    //if(client->asynchronous(true, true) == -1){
    //changed:
    if(client->asynchronous(true, false) == -1){
        //only close, the group will catch and close the channel
        client->close();        
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input - failed async!\n")));
        return -1;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO FTControlAcceptor::handle_input -done!\n")));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int FTControlAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void FTControlAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //m_scvHandlers.remove(channel);
}

int FTControlAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);
}

EndpointPtr& FTControlAcceptor::getEndpoint() {
    return m_endpointPtr;
}
