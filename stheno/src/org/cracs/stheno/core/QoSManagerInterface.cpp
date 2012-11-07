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
 * File:   QoSManagerInterface.cpp
 * Author: rmartins
 * 
 * Created on January 12, 2011, 5:36 PM
 */

#include "QoSManagerInterface.h"
#include <euryale/qos/medusa/Cgroup.h>

#include <stheno/core/SthenoCore.h>
#include <stheno/core/QoSManager.h>

QoSManagerInterface::QoSManagerInterface(SthenoCore* core, String& rootPath, 
        ULong runtime, ULong period,bool create) throw (CgroupException&) :
m_core(core), m_rootPath(rootPath) {    
    CPUGroupQoS qos(Cgroup::getHRTBase(), rootPath, runtime, period, 1024);
    if(create){
        m_core->getQoSManager().createGroupCPUReserve(rootPath, qos);
    }
}


QoSManagerInterface::QoSManagerInterface(SthenoCore* core, const char* rootPath, 
        ULong runtime, ULong period,bool create) throw (CgroupException&) :
m_core(core), m_rootPath(rootPath) {        
    CPUGroupQoS qos(Cgroup::getHRTBase(), m_rootPath, runtime, period, 1024);
    if(create){
        m_core->getQoSManager().createGroupCPUReserve(m_rootPath, qos);
    }
}

QoSManagerInterface::~QoSManagerInterface() {
}


QoSManagerInterface* QoSManagerInterface::createSubDomainQoSManager(String& path,ULong runtime,ULong period) throw (CgroupException&) {
    String fullPath(getRootPath());
    if(Cgroup::FLAT_HIERARCHY){
        if(path != ""){
           FlatHierarchicalPath::appendPaths(fullPath, path);
        }
    }else{
        HierarchicalPath::appendPaths(fullPath, path);
    }
    QoSManagerInterface* qosManager = new QoSManagerInterface(m_core,fullPath,runtime,period);
    return qosManager;
}

QoSManagerInterface* QoSManagerInterface::createSubDomainQoSManager(const char* path,ULong runtime,ULong period) throw (CgroupException&) {
    String pathStr(path);
    return createSubDomainQoSManager(pathStr,runtime,period);
}


CPUReservation* QoSManagerInterface::createCPUReservation(
        String& cpusetDomain,
        //String& scheduleDomain,
        CPUQoS* qos) throw (CgroupException&) {
    //String fullPath(getRootPath());
    //HierarchicalPath::appendPaths(fullPath, scheduleDomain);
    //m_core->getQoSManager().createCPUReservation(cpusetDomain, fullPath, qos);  
    if(!Cgroup::PARTITIONING){
        cpusetDomain = "";
    }
    return m_core->getQoSManager().createCPUReservation(cpusetDomain, m_rootPath, qos);    
}

CPUReservation* QoSManagerInterface::createCPUReservation(
        const char* cpusetDomain,
        //String& scheduleDomain,
        CPUQoS* qos) throw (CgroupException&) {
    String cpusetDomainPath(cpusetDomain);
    if(!Cgroup::PARTITIONING){
        cpusetDomainPath = "";
    }
    //String fullPath(getRootPath());
    //HierarchicalPath::appendPaths(fullPath, scheduleDomain);
    //m_core->getQoSManager().createCPUReservation(cpusetDomain, fullPath, qos);    
    printf("====> TAG 1 %p\n",qos);
    return m_core->getQoSManager().createCPUReservation(cpusetDomainPath, m_rootPath, qos);
   
}

list<CPUPartition*>* QoSManagerInterface::getCPUPartitionList() throw (CgroupException&) {
    return m_core->getQoSManager().getCPUPartitionList();
}

void QoSManagerInterface::createGroupCPUReserve(
        String& runtimeUUID,
        CPUGroupQoS& qos) throw (CgroupException&) {
    m_core->getQoSManager().createGroupCPUReserve(runtimeUUID, qos);
}

void QoSManagerInterface::setTaskInCPUPartition(
        UInt task,
        String& cpusetGroup,
        //String& scheDomain,
        CPUQoS* qos) throw (CgroupException&) {
    //String fullPath(getRootPath());
    //HierarchicalPath::appendPaths(fullPath, scheDomain);
    
    m_core->getQoSManager().setTaskInCPUPartition(task, cpusetGroup, m_rootPath, qos);
}

String& QoSManagerInterface::getRootPath() {
    return m_rootPath;
}