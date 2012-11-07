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
 * File:   CellAceptor.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 9, 2010, 12:58 PM
 */

#include "CellAcceptor.h"
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellSvcHandler.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellFollowerClientHandler.h>
#include <stheno/core/QoSManagerInterface.h>

CellAcceptor::CellAcceptor(Cell* cell,
        ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>* cellGroup,
        ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>* followerGroup,
        ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>* childrenGroup,
        ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>* leafGroup) :
m_cell(cell),
m_cellGroup(cellGroup),
m_cellFollowerGroup(followerGroup),
m_childrenGroup(childrenGroup),
m_leafGroup(leafGroup),
m_tpcPrt(0) {
}

CellAcceptor::~CellAcceptor() {
}

int CellAcceptor::open(void *arg) {
    throw -1;
}

int CellAcceptor::close() {
    m_acceptor.close();
    m_cell = 0;
    if (m_tpcPrt != 0) {
        delete m_tpcPrt;
        m_tpcPrt = 0;
    }
    return 0;
}

int CellAcceptor::open(EndpointPtr& endpointPtr) throw (EndpointException&, ExecutionModelException&) {
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

ACE_HANDLE CellAcceptor::get_handle(void) const {
    return this->m_acceptor.get_handle();
}

// Called when a connection is ready to accept.

int CellAcceptor::handle_input(ACE_HANDLE fd) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellAcceptor::handle_input - New incoming connection\n")));
    CellSvcHandler *client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new ExecutionModelPtr (tpc);
    ACE_NEW_RETURN(client,
            CellSvcHandler(m_cell,  tpcPrt, 0, 0, 0), -1);

    auto_ptr<CellSvcHandler> p(client);
    if (this->m_acceptor.accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
            ACE_TEXT("(%T) failed to accept\n")),
            -1);
    p.release();

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO CellAcceptor::handle_input - new connection %d!\n"), client->get_handle()));
    CPUPriorityQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (m_cell->getQoSManager() != 0) {
        reserve = m_cell->getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    //printf("==================== %p %p \n\n\n\n",cpuQoS,reserve);
    tpc->bind(client);
    tpc->open(reserve, cpuQoS);

    if (client->open() == -1) {
        printf("CellAcceptor: failed to eopn client == -1\n");
        delete client;
        return 0;
    }
    UUIDPtr uuid;
    m_cell->getPID(uuid);
    UUIDPtr fid;
    m_cell->getFID(fid);
    CellIDPtr cellID;
    m_cell->getCellID(cellID);
    if (client->isLeaf()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellAcceptor::handle_input() - Add new children peer (%s,%s)\n"),
                cellID->toString().c_str(), client->getCellID()->toString().c_str()));
        if(m_leafGroup->add(client) == -1){
            return 0;
        }
    } else {
        if (client->isChildren()) {
            CellIDPtr runtimeCellID;
            m_cell->getCellID(runtimeCellID);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellAcceptor::handle_input() - Add new children peer (%s,%s)\n"),
                    runtimeCellID->toString().c_str(), client->getCellID()->toString().c_str()));
            if(m_childrenGroup->add(client)==-1){
                return 0;
            }
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellAcceptor::handle_input() - Add group peer\n")));
            if(m_cellGroup->add(client) == -1){
                return 0;
            }
            ACE_Connector<CellFollowerClientHandler, ACE_SOCK_Connector> connector;
            ThreadPerConnection *tpc = new ThreadPerConnection();
            //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
            //        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
            ExecutionModelPtr* tpcPrt = new ExecutionModelPtr(tpc);

            CellFollowerClientHandler* followerClient = new CellFollowerClientHandler(
                    client->getPID(), client->getFID(), m_cell,
                    //true, false,
                    false,false,
                    tpcPrt,
                    0, 0, 0);

            CPUQoS* followerCPUQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
            CPUReservation* followerReserve = 0;
            if (m_cell->getQoSManager() != 0) {
                followerReserve = m_cell->getQoSManager()->createCPUReservation("HRT", cpuQoS);
            }

            tpc->bind(followerClient);
            tpc->open(followerReserve,followerCPUQoS);
            connector.reactor(tpc->getResources()->getReactor());

            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Connecting to follower...(%s)!\n"), client->getPID()->toString().c_str()));

            if (connector.connect(followerClient, client->getFollowerEndpoint()->getAddr()) == -1) {
                ACE_ERROR((LM_ERROR, ACE_TEXT("CellAcceptor::handle_input follower connection failed\n")));
                followerClient->close();
                throw ServiceException("Error connecting to follower!");
            } else {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connect OK!\n")));
            }

            if(m_cellFollowerGroup->add(followerClient) == -1){
                client->close();
                return 0;
            }
            if (followerClient->asynchronous(true, false)) {
                followerClient->close();
            }
        }
    }
    //if (client->asynchronous(true, true) == -1) {
    //changed:
    if (client->asynchronous(true, false) == -1) {
        client->close();
    }

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: client binded OK!\n")));
    return 0;
}

// Called when this handler is removed from the ACE_Reactor.

int CellAcceptor::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (this->m_acceptor.get_handle() != ACE_INVALID_HANDLE) {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK |
                ACE_Event_Handler::DONT_CALL;
        this->reactor()->remove_handler(this, m);
        this->m_acceptor.close();
    }
    return 0;
}

void CellAcceptor::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    //TODO: check this
    /*if (m_cellGroup != 0) {
        m_cellGroup->remove(channel);
    }*/
}

int CellAcceptor::getListenAddr(ACE_INET_Addr& addr) {
    return m_acceptor.get_local_addr(addr);

}

EndpointPtr& CellAcceptor::getEndpoint() {
    return m_endpointPtr;
}