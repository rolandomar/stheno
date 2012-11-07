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
 * File:   SthenoCoreInterface.cpp
 * Author: rmartins
 * 
 * Created on October 21, 2010, 4:33 PM
 */

#include "SthenoCoreInterface.h"
#include <stheno/core/SthenoCore.h>

SthenoCoreInterface::SthenoCoreInterface(SthenoCore* core) : m_core(core) {
    //m_qosManager = m_core->getQoSManagerInterface(path,runtime,period);
}

SthenoCoreInterface::~SthenoCoreInterface() {
}

void SthenoCoreInterface::createLocalService(UUIDPtr& sid, ServiceParamsPtr& params,UUIDPtr& iid) throw (ServiceException&) {
    m_core->createLocalService(sid, params,iid);
}


void SthenoCoreInterface::createReplicationGroup(
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& peers,
            UUIDPtr& rgid,
            UUIDPtr& sid,            
            ServiceParamsPtr& params            
            ) throw (ServiceException&){
    m_core->createReplicationGroup(primary,peers,rgid,sid,params);
}



void SthenoCoreInterface::createLocalServiceReplica(UUIDPtr& sid, ServiceParamsPtr& params,ServiceAbstractPtr& sPtr) throw (ServiceException&){
    m_core->createLocalServiceReplica(sid, params,sPtr);
}

void SthenoCoreInterface::changeIIDOfService(UUIDPtr& sid,UUIDPtr& iid,UUIDPtr& newIid) throw (ServiceException&){
    m_core->changeIIDOfService(sid,iid,newIid);
}

void SthenoCoreInterface::stopLocalService(UUIDPtr& sid, UUIDPtr& iid) throw (ServiceException&) {
    m_core->stopLocalService(sid, iid);
}

bool SthenoCoreInterface::isServiceRunning(UUIDPtr& sid, UUIDPtr& idd) {
    return m_core->isServiceRunning(sid, idd);
}

list<UUIDPtr>* SthenoCoreInterface::getInstancesOfServiceUUIDs(UUIDPtr& sid) throw (ServiceException&) {
    return m_core->getInstancesOfServiceUUIDs(sid);
}

list<ServiceInstanceInfoPtr>* SthenoCoreInterface::getInstancesOfService(UUIDPtr& sid) throw (ServiceException&) {
    return m_core->getInstancesOfService(sid);
}

void SthenoCoreInterface::getInstancesOfService(UUIDPtr& sid,UUIDPtr& iid,ServiceInstanceInfoPtr& info) throw (ServiceException&) {
    return m_core->getInstanceOfService(sid,iid,info);
}

/*void SthenoCoreInterface::allocateQoS(QoSResources* qos) throw (ServiceException&) {
    m_core->allocateQoS(qos);
}*/

void SthenoCoreInterface::getUUID(UUIDPtr& uuid) {
    m_core->getUUID(uuid);
}

bool SthenoCoreInterface::isValid() {
    return m_core->isValid();
}

bool SthenoCoreInterface::isSuspended() {
    return m_core->isSuspended();
}

bool SthenoCoreInterface::isClose() {
    return m_core->isClose();
}

Byte SthenoCoreInterface::getStatus() {
    return m_core->getStatus();
}

ServiceFactory& SthenoCoreInterface::getDefaultServiceFactory(){
    return m_core->getServiceFactory();
}

UInt SthenoCoreInterface::getRuntimeQoS(){
    return m_core->getRuntimeQoS();    
}

UInt SthenoCoreInterface::getRuntimeQoSPeriod(){
    return m_core->getRuntimeQoSPeriod();
    
}


/*QoSManagerInterface* SthenoCoreInterface::getQoSManager(){
    return m_qosManager;
}*/