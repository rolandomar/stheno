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
 * File:   MedusadCore.cpp
 * Author: rmartins
 * 
 * Created on November 28, 2009, 4:34 PM
 */

#include "MedusadCore.h"
#include "net/MedusadAcceptor.h"
#include <euryale/qos/medusa/Cgroup.h>
#include <euryale/qos/medusa/net/MedusaServiceHandler.h>
#include <euryale/qos/net/packet/MedusaPacket.h>

#include <euryale/qos/net/packet/ThreadReserveRequestPacket.h>
#include <euryale/qos/net/packet/ThreadReserveReplyPacket.h>
#include <euryale/qos/net/packet/GetPartitionRequestPacket.h>
#include <euryale/qos/net/packet/GetPartitionReplyPacket.h>

#include <euryale/qos/net/packet/CPUGroupReserveRequestPacket.h>
#include <euryale/qos/net/packet/CPUGroupReserveReplyPacket.h>

MedusadCore::MedusadCore() : m_acceptor(0) {
}

MedusadCore::MedusadCore(const MedusadCore& orig) :
m_acceptor(0) {
}

MedusadCore::~MedusadCore() {
    if (m_cg != 0) {
        delete m_cg;
        m_cg = 0;
    }
}

void MedusadCore::close() {
    delete m_acceptor;
    m_stop = true;
    m_cg->close();
}

void MedusadCore::run() {    
    while (!m_stop) {
        /// read command
        //ACE_Message_Block* packet = 0;
        //ACE_Message_Block* reply = processPacket(packet);
        // send(reply)
        //ACE_Time_Value timeout()
        ACE_Reactor::instance()->handle_events();
    }
}

void MedusadCore::remove() throw (CgroupException&) {
    m_cg = new Cgroup();
    m_cg->remove();
}

void MedusadCore::init(bool onlyReset) throw (CgroupException&) {
    //remove();
    m_acceptor = new MedusadAcceptor(this);
    m_stop = false;
    m_cg = new Cgroup();
    printf("MedusaCore::init() - about to reset...\n");
    m_cg->init(onlyReset);
    printf("MedusaCore::init() - reseted\n");
    if (!onlyReset) {
        m_acceptor->reactor(ACE_Reactor::instance());
        if (m_acceptor->open() != 0) {
            throw CgroupException("MedusadCore::init() Acceptor failed to init!");
        }
        printf("MedusadCore::init() - acceptor is ok\n");
    }
    printf("MedusadCore::init() - end\n");
    String d;    
    m_cg->getCPUSetGroup()->setTask(d.c_str(), gettid());    
}

void MedusadCore::start() {
    run();
}

void MedusadCore::processPacket(MedusaPacket* packet, MedusaServiceHandler* handler) {
    MedusaHeader* header = static_cast<MedusaHeader*> (packet->getPacketHeader());
    switch (header->getType()) {
        case GET_PARTITION_REQUEST_PACKET_ID:
        {
            list<CPUPartition*>* partitionList = this->getCPUPartitions();
            GetPartitionReplyPacket* reply =
                    new GetPartitionReplyPacket(0, header->getPacketID(), partitionList);
            handler->send(reply);
            delete partitionList;
            return;
        }

        case THREAD_RESERVE_REQUEST_PACKET_ID:
        {
            ThreadReserveReplyPacket* reply = 0;
            ThreadReserveRequestPacket* packetImpl = static_cast<ThreadReserveRequestPacket*> (packet);
            UInt runtime = 0;
            UInt period = 0;
            switch (packetImpl->getCPUQoS()->getPolicy()) {
                case CPUQoS::SCHEDULE_DEADLINE:
                {
                    CPUDeadlineQoS* qos = static_cast<CPUDeadlineQoS*> (packetImpl->getCPUQoS());
                    runtime = qos->getRuntime();
                    period = qos->getPeriod();
                    break;
                }
            }
            try {
                this->setTaskInCPUReserve(packetImpl->getTask(), packetImpl->getCgroup(), packetImpl->getScheGroup());
                reply =
                        new ThreadReserveReplyPacket(0, header->getPacketID(), 0);
            } catch (CgroupException* ex) {
                delete ex;
                reply =
                        new ThreadReserveReplyPacket(0, header->getPacketID(), -1);

            }
            handler->send(reply);
            return;
        }


        case CPUGROUP_RESERVE_REQUEST_PACKET_ID:{
            CPUGroupReserveReplyPacket* reply = 0;
            CPUGroupReserveRequestPacket* packetImpl = static_cast<CPUGroupReserveRequestPacket*> (packet);
            try {
                this->createRuntimeReservation(packetImpl->getCgroup(),
                        packetImpl->getCPUGroupQoS().getRuntime(),
                        packetImpl->getCPUGroupQoS().getPeriod());
                reply =
                        new CPUGroupReserveReplyPacket(0, header->getPacketID(), 0);
            } catch (CgroupException* ex) {
                delete ex;
                reply =
                        new CPUGroupReserveReplyPacket(0, header->getPacketID(), -1);

            }
            handler->send(reply);
            return;
        }
        default:
        {
            ACE_DEBUG((LM_DEBUG,
                    ACE_TEXT("(%T) Unknow packet! %s\n"), packet->getPacketHeader()->getType()));
            delete packet;            
        }
    }
}

void MedusadCore::onCloseHandle(MedusaServiceHandler* handler) {
    String path;    
    //m_cg->getCPUSchedGroup()->getCgroupDirectory(path);
    //HierarchicalPath::appendPaths(path,"/runtime_"+handler->getUUID()->toString());    
    //m_cg->getCPUSchedGroup()->removeSubDomain(true,path.c_str());
     
    //path += "/"+handler->getUUID()->toString();
    //m_cg->getCPUSchedGroup()->reset(path.c_str());    
    m_cg->getCPUSetGroup()->getCgroupDirectory(path);        
    HierarchicalPath::appendPaths(path,"HRT");
    HierarchicalPath::appendPaths(path,"/runtime_"+handler->getUUID()->toString());
    m_cg->getCPUSetGroup()->removeSubDomain(true,path.c_str());
    
    //TODO: disabling SRT
    /*m_cg->getCPUSetGroup()->getCgroupDirectory(path);        
    HierarchicalPath::appendPaths(path,"SRT");
    HierarchicalPath::appendPaths(path,"/runtime_"+handler->getUUID()->toString());    
    m_cg->getCPUSetGroup()->removeSubDomain(true,path.c_str());*/
    
    
    m_cg->getCPUSetGroup()->getCgroupDirectory(path);        
    HierarchicalPath::appendPaths(path,"BE");
    HierarchicalPath::appendPaths(path,"/runtime_"+handler->getUUID()->toString());    
    m_cg->getCPUSetGroup()->removeSubDomain(true,path.c_str());
    
   
    //ACE_OS::rmdir(path.c_str());
    printf("Done cleanup - %s\n",path.c_str());
}

void MedusadCore::onOpenHandle(MedusaServiceHandler* handler) {

}

