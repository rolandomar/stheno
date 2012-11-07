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
 * File:   RPCService.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 2:25 PM
 */

#include "RPCService.h"
#include "RPCTestObject.h"
#include "stheno/Stheno.h"
#include <stheno/services/rpc/RPCServiceClient.h>
#include <stheno/core/p2p/p3/P3Config.h>

RPCService::RPCService() : UserService(0, id()), m_testObject(this),m_runtimeManagerObject(this), m_sap(0) {
}

RPCService::RPCService(Stheno* runtime) : UserService(runtime, id()), m_testObject(this), m_runtimeManagerObject(this),m_sap(0) {
}

RPCService::RPCService(Stheno* runtime, UUIDPtr& iid) : UserService(runtime, id(), iid), m_testObject(this), m_runtimeManagerObject(this),m_sap(0) {
}

RPCService::~RPCService() {
}

const char* RPCService::id() {
    return "3355C3F005459F672057661FBE9C70BB";
}

void RPCService::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    m_ftType = fttype;
    list<QoSEndpoint> qosList;
    QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    //QoSEndpoint low(QoS::SRT, CPUQoS::LOW_RT_PRIO);
    QoSEndpoint srt(QoS::SRT, CPUQoS::MED_RT_PRIO);
    //QoSEndpoint srt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    QoSEndpoint be(QoS::BE, CPUQoS::BE_PRIO);
    
    qosList.push_back(hrt);
    //qosList.push_back(low);
    qosList.push_back(srt);
    qosList.push_back(be);
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i(%d) creating SAP... exception!\n"), m_status));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 2....\n"), m_status));
    m_sap = new RPCServiceSAP(this);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 3....\n"), m_status));
    m_sap->open(itf, qosList);

}

void RPCService::close() {

}

ServiceClient* RPCService::getClient(const UUIDPtr& sid,ClientParamsPtr& cp) {
    return 0;
}

ServiceClient* RPCService::getClient(const UUIDPtr& sid, const UUIDPtr& iid,ClientParamsPtr& cp) {
    RPCServiceClient* rpcServiceClient = new RPCServiceClient(m_runtime->duplicate(), this);
    if (rpcServiceClient->open(sid, iid,cp) != 0) {
        delete rpcServiceClient;
        return 0;
    }
    return rpcServiceClient;
}

QoSResources* RPCService::calculateQoSResources(ServiceParamsPtr& params) {
    return 0;
}

ServiceAbstract* RPCService::duplicate() throw (ServiceException&) {
    return new RPCService(m_runtime->duplicate());
}

const char* RPCService::getName() {
    return "RPCService";
}

void RPCService::getSAP(SAPInfoPtr& sap) {
    sap = m_sap->getSAPInfo();
}

void RPCService::handleOneWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) {    
    if (oid->isEqual(m_testObject.getOID())) {
        m_testObject.handleOneWayInvocation(arg);
        return;
    }
    if (oid->isEqual(m_runtimeManagerObject.getOID())){
        m_runtimeManagerObject.handleOneWayInvocation(arg);
        return;
    }    
}

ACE_Message_Block* RPCService::handleTwoWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) {
    if (oid->isEqual(m_testObject.getOID())) {
        ACE_Message_Block* reply = m_testObject.handleTwoWayInvocation(arg);        
        return reply;
    }
    if (oid->isEqual(m_runtimeManagerObject.getOID())){
        ACE_Message_Block* reply = m_runtimeManagerObject.handleTwoWayInvocation(arg);
        return reply;
    }
    ACE_Message_Block::release(arg);
    return 0;
}

void RPCService::onReplication(ACE_Message_Block* replicationData) throw (ServiceException&) {
    //printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Replicaiton>>>>>>>>>>>>>>>>\n");
    ByteInputStream is(replicationData);
    ACE_Message_Block::release(replicationData);    
    m_testObject.onReplication(is);            
}
