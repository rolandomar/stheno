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
 * File:   QoSManager.cpp
 * Author: rmartins
 * 
 * Created on January 6, 2011, 4:39 PM
 */

#include "QoSManager.h"
#include <euryale/threading/Task.h>
#include <stheno/core/SthenoCore.h>

QoSManager::QoSManager(SthenoCore* runtime) :
m_runtime(runtime), m_closed(false), m_started(false), m_medusaClient(0) {
}

QoSManager::~QoSManager() {
    //ensure that client is properly closed
    close();
}

void QoSManager::onMedusaClientClose() {
    printf("QoSManager::onMedusaClientClose()\n");
    m_runtime->onRuntimeError();
}

void QoSManager::close() {
    if (m_closed) {
        return;
    }
    m_closed = true;
    /*if(m_medusaClient != 0){
        m_medusaClient->close();
        delete m_medusaClient;
        m_medusaClient = 0;
    }*/
}

void QoSManager::open() throw (CgroupException&) {
    if (m_started) {
        return;
    }
    m_started = true;
    try {
        //GlobalMedusaClient::instance()->open();
        /*m_medusaClient = new MedusaClient();
        m_medusaClient->setCloseListener(this);
        m_medusaClient->open(m_runtime->getUUID());*/
        m_medusaClient = GlobalMedusaClient::instance();
        m_medusaClient->setCloseListener(this);
        UUIDPtr uuid;
        m_runtime->getUUID(uuid);
        m_medusaClient->open(uuid);
    } catch (CgroupException&) {
        //throw RuntimeException and abort
        //delete m_medusaClient;
        m_medusaClient = 0;
        m_started = false;
        throw CgroupException("Medusa server not available");
    }
    UUIDPtr runtimeUUID;
    m_runtime->getUUID(runtimeUUID);
    String runtimeDomainPath = "runtime_" + runtimeUUID->toString();

    String cpusetHRTDomainPath("HRT/");
    //String cpusetHRTDomainPath("/");
    //cpusetHRTDomainPath += runtimeDomainPath;

    String scheduleHRTDomainPath(runtimeDomainPath);
    //QoS for root of runtime's reservation group
    CPUGroupQoS qos(cpusetHRTDomainPath, scheduleHRTDomainPath, 
            m_runtime->getRuntimeQoS(), CPUReservation::DEFAULT_PERIOD, 1024);    
    //create the reservation group for this runtime
    //TODO: not create reserve
    m_medusaClient->createGroupCPUReserve(scheduleHRTDomainPath, qos);

    //TODO: change this
    //check if this thread is a BE and convert it
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    Task::setThreadScheduling(ACE_OS::thr_self(),cpuQoS);
    GlobalMedusaClient::instance()->setTaskInCPUPartition(gettid(),
            cpusetHRTDomainPath, scheduleHRTDomainPath, cpuQoS);
    
    //Reserve for the QOS client!!!!!!!!!!!!!1
    CPUQoS* cpuQoSClient = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    Task::setThreadScheduling(m_medusaClient->getThreadHandler(),cpuQoSClient);
    GlobalMedusaClient::instance()->setTaskInCPUPartition(m_medusaClient->getTid(),
            cpusetHRTDomainPath, scheduleHRTDomainPath, cpuQoSClient);


}

CPUReservation* QoSManager::createCPUReservation(
        String& cpusetDomain, String& scheduleDomain, CPUQoS* qos) throw (CgroupException&) {
    CPUReservation* reserve = m_medusaClient->createCPUReservation(cpusetDomain, scheduleDomain, qos);
    return reserve;
}

list<CPUPartition*>* QoSManager::getCPUPartitionList() throw (CgroupException&) {
    return m_medusaClient->getCPUPartitionList();
}

void QoSManager::createGroupCPUReserve(String& runtimeUUID, CPUGroupQoS& qos) throw (CgroupException&) {
    m_medusaClient->createGroupCPUReserve(runtimeUUID, qos);
}

void QoSManager::setTaskInCPUPartition(UInt task, String& cpusetGroup,
        String& scheDomain, CPUQoS* qos) throw (CgroupException&) {
    m_medusaClient->setTaskInCPUPartition(task, cpusetGroup, scheDomain, qos);
}