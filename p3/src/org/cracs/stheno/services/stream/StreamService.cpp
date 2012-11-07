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
 * File:   StreamService.cpp
 * Author: rmartins
 * 
 * Created on October 26, 2010, 11:29 AM
 */

#include "StreamService.h"
#include "stheno/Stheno.h"
#include <stheno/core/p2p/p3/leaf/stream/net/StreamClientHandler.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/services/stream/net/StreamServiceSAP.h>
#include <stheno/core/p2p/p3/P3Config.h>

StreamService::StreamService(Stheno* runtime, const char* id) :
UserService(runtime, id),
m_streamParams(0), m_sap(0) {
    //10 minutes of storage space
    m_storageMaxLimit = 24 * 60 * 10;
    //10 s
    //m_storageMaxLimit = 24 * 10;
    m_storageIndex = 0;
    for (int i = 0; i < m_storageMaxLimit; i++) {
        //m_storage[i] = 0;
        m_storage.push_back(0);
    }
}

StreamService::StreamService(Stheno* runtime, const char* id, UUIDPtr& iid) :
UserService(runtime, id, iid),
m_streamParams(0), m_sap(0) {    
    //10 minutes of storage space
    m_storageMaxLimit = 24 * 60 * 10;
    //10 s
    //m_storageMaxLimit = 24 * 10;
    m_storageIndex = 0;
    for (int i = 0; i < m_storageMaxLimit; i++) {
        //m_storage[i] = 0;
        m_storage.push_back(0);
    }
}

StreamService::~StreamService() {
}

void StreamService::getSAP(SAPInfoPtr& sap) {
    sap= m_sap->getSAPInfo();
}

void StreamService::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    //m_streamParams = static_cast<ServiceStreamParams*>(params);
    list<QoSEndpoint> qosList;
    //QoSEndpoint hrt(QoS::HRT, 50);
    //QoSEndpoint srt(QoS::SRT, 25);
    QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    QoSEndpoint srt(QoS::SRT, CPUQoS::MED_RT_PRIO);
    QoSEndpoint be(QoS::BE, CPUQoS::BE_PRIO);
    qosList.push_back(hrt);
    qosList.push_back(srt);
    qosList.push_back(be);
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i(%d) creating SAP... exception!\n"), m_status));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 2....\n"), m_status));
    m_sap = new StreamServiceSAP(this);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 3....\n"), m_status));
    m_sap->open(itf, qosList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)StreamService::open_i -  creating SAP 3.... %@\n"), params->getQuerySpecializationBlob()));


    m_streamParams = new ServiceStreamParams(params.get()->duplicate());
    EndpointPtr endpoint = m_streamParams->getEndpoint();
    ACE_Connector<StreamClientHandler, ACE_SOCK_Connector> connector;
    //QoSEndpoint qosE = *(endpoint.getQoS());
    m_overlayInterface = m_runtime->getOverlayInterface();

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
      //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr (tpc);
    UUIDPtr runtimeUUID;
    UUIDPtr fid;
    m_runtime->getUUID(runtimeUUID);
    m_overlayInterface->getMesh()->getFID(fid);
    
    m_sensorClient = new StreamClientHandler(
            runtimeUUID,
            fid,
            m_streamParams->getQoS()->getFrameSize(),
            m_streamParams->getQoS()->getFrameRate(),
            false, false, tpcPrt, 0, 0, 0);
    m_sensorClient->setFrameListener(this);
    tpc->bind(m_sensorClient);
    tpc->open();
    //tpc->bind(m_sensorClient);
    connector.reactor(tpc->getResources()->getReactor());

    if (connector.connect(m_sensorClient, endpoint.get()->getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: SuperPeerDiscovery::executeQuery() - connect failed:")));
        m_sensorClient->close();
        delete m_sensorClient;
        m_sensorClient = 0;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamService::open_i() - Connect to SENSOR OK!\n")));

    }    
    if(m_sensorClient->asynchronous(true, false) == -1){
        m_sensorClient->close();
        delete m_sensorClient;
        m_sensorClient = 0;
    }
}

void StreamService::addToStorage(FramePacket* frame) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock());
    UInt realPos = m_storageIndex%m_storageMaxLimit;
    //if(m_storageIndex!=0){
        //realPos = m_storageIndex%m_storageMaxLimit;
    //}    
    //printf("\n\n Add storage%d\n",realPos);
    if (m_storage[realPos] != 0) {
        delete m_storage[realPos];
    }
    m_storage[realPos] = frame;
    m_storageIndex++;
}

int StreamService::getRetrieveStorage(UInt frameNo, list<StreamFramePacket*>& l) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, getLock(),-1);
    /*UInt realPos = 0;
    if(m_storageIndex!=0){
        realPos = m_storageMaxLimit % m_storageIndex;
    }*/
    UInt realPos = m_storageIndex%m_storageMaxLimit;
    
    //printf("\n\n\n LastFrame=%d\n\n\n",frameNo);
    int size = m_storage.size();
    for (int i = 0; i < size; i++) {
        if (m_storage[i] != 0) {
            //printf("Search %d %d %d\n",i,m_storage[i]->getFrameNo(),frameNo);
            if (m_storage[i]->getFrameNo() == frameNo) {
                //found!
                if (i < realPos) {
                    //printf("(i < realPos)\n");
                    for (int index = i; index < realPos; index++) {
                        StreamFramePacket* packet = new StreamFramePacket(0, m_storage[index]->getFrameNo(),
                                m_storage[index]->getFrameSize(),m_storage[index]->getTimestamp() ,m_storage[index]->getPayload()->duplicate());
                        l.push_back(packet);
                    }
                    //printf("STORED=%lu\n",l.size());
                    return 0;
                } else {
                    //printf("(i >= realPos)\n");
                    for (int index = i; index < m_storageMaxLimit; index++) {
                        StreamFramePacket* packet = new StreamFramePacket(0, m_storage[index]->getFrameNo(),
                                m_storage[index]->getFrameSize(),m_storage[index]->getTimestamp() ,m_storage[index]->getPayload()->duplicate());
                        l.push_back(packet);
                    }
                    for (int index = 0; index < realPos; index++) {
                        StreamFramePacket* packet = new StreamFramePacket(0, m_storage[index]->getFrameNo(),
                                m_storage[index]->getFrameSize(),m_storage[index]->getTimestamp() ,m_storage[index]->getPayload()->duplicate());
                        l.push_back(packet);
                    }
                    return 0;
                }
            }
        }
    }
    return -1;
}

void StreamService::onFramePacket(FramePacket* packet) {
    //packet is owned by storage, so do not delete it here!
    addToStorage(packet);
    list<StreamServiceAcceptor*>& l = m_sap->getAcceptorList();
    list<StreamServiceAcceptor*>::iterator iter = l.begin();
    //printf("Sending frame to clientS...\n");
    while (iter != l.end()) {
        StreamFramePacket* p = new StreamFramePacket(0, packet->getFrameNo(), packet->getFrameSize(),packet->getTimestamp(),packet->getPayload()->duplicate());
        //printf("Sending frame client...\n");
        (*iter)->getChannelManager().sendFrame(p);
        //printf("Sent frame client...\n");
        iter++;
        delete p;
    }    
    //printf("Sending frame to clientS...\n");
    
}

bool StreamService::changeToPrimaryRole() {
    return true;
}