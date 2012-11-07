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

#include "MedusaClient.h"
#include <ace/Reactor_Impl.h>
#include <ace/TP_Reactor.h>
#include "ace/OS_NS_sys_time.h"
#include <euryale/qos/MedusaConf.h>
#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>

MedusaClient::MedusaClient() :
m_listener(0),
m_clientHandler(0),
m_reactor(0),
m_tid(0),
m_th(0){
}

MedusaClient::~MedusaClient() {
    /*if (m_clientHandler != 0) {
        m_clientHandler->close();
        delete m_clientHandler;
    }
    if (m_reactor != 0) {
        m_reactor->end_reactor_event_loop();
        delete m_reactor;
    }*/
    close();
}

void MedusaClient::setCloseListener(MedusaClientCloseListener* listener){
    m_listener = listener;
}

void MedusaClient::onMedusaClientHandlerClose(){    
    if (m_reactor != 0) {
        m_reactor->end_reactor_event_loop();       
        //must be delete on thread
        //delete m_reactor;
    }
    if(m_listener!=0){
        m_listener->onMedusaClientClose();
    }
}

void MedusaClient::open(UUIDPtr& uuid) throw (CgroupException&) {
    if (m_clientHandler != 0) {
        throw CgroupException("Already started!");
    }
    m_reactor = allocateReactorImpl();
    ACE_UNIX_Addr addr(MedusaConf::MedusadPath);
    ACE_Connector<MedusaClientHandler, ACE_LSOCK_CONNECTOR> connector;
    printf("MedusaClient::open - Reactor=%p\n", m_reactor);
    m_clientHandler = new MedusaClientHandler(uuid,this);
    connector.reactor(m_reactor);
    //m_clientHandler->reactor(m_reactor);

    if (connector.connect(m_clientHandler, addr) == -1) {
        perror("Error: MedusaClient::open():");
        m_clientHandler->close();
        throw CgroupException("Couldn't connect to medusad");
    }

    if (this->activate() != 0) {
        throw CgroupException("Error creating thread");
    }
}

void MedusaClient::close() {
    if (m_clientHandler != 0) {
        m_clientHandler->close();
        delete m_clientHandler;
        m_clientHandler = 0;
    }
    if (m_reactor != 0) {
        m_reactor->end_reactor_event_loop();
    }
}

int MedusaClient::svc(void) {
    m_tid = gettid();
    m_th = ACE_OS::thr_self();
    m_reactor->owner(ACE_OS::thr_self());
    m_reactor->run_reactor_event_loop();        
    delete m_reactor;
    return 0;
}

ACE_Reactor* MedusaClient::allocateReactorImpl(void) {
    ACE_Reactor_Impl *impl = 0;
    ACE_NEW_RETURN(impl,
            ACE_TP_Reactor(ACE::max_handles(),
            1,
            (ACE_Sig_Handler*) 0,
            (ACE_Timer_Queue*) 0,
            0,
            ACE_Select_Reactor_Token::LIFO),
            0);
    return (ACE_Reactor*) impl;
}

list<CPUPartition*>* MedusaClient::getCPUPartitionList() throw (CgroupException&) {
    return m_clientHandler->getCPUPartitionList();
}

CPUReservation* MedusaClient::createCPUReservation(
        String& cpusetDomain, String& scheduleDomain, CPUQoS* qos) throw(CgroupException&){
    //Should be done by server, but we arent doing admission control
    printf("====> TAG 3\n");
    CPUReservation* reserve = new CPUReservation(cpusetDomain,scheduleDomain,qos->duplicate());
    printf("====> TAG 3 %p %p\n",qos,reserve);
    printf("cpusetDomain=%s scheduleDomain=%s\n",cpusetDomain.c_str(),scheduleDomain.c_str());
    return reserve;
}