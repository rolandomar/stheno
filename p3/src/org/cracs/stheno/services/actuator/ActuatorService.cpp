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
 * File:   ActuatorService.cpp
 * Author: rmartins
 * 
 * Created on October 26, 2010, 11:29 AM
 */

#include "ActuatorService.h"
#include "stheno/Stheno.h"
#include <stheno/core/p2p/p3/leaf/actuator/net/ActuatorClientHandler.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/services/actuator/net/ActuatorServiceSAP.h>
#include <stheno/services/actuator/ActuatorServiceClient.h>
#include <stheno/core/p2p/p3/P3Config.h>

ActuatorService::ActuatorService() : UserService(0, id()), m_actuatorParams(0), m_sap(0) {
}

ActuatorService::ActuatorService(Stheno* runtime) : UserService(runtime, id()), m_actuatorParams(0), m_sap(0) {
}

ActuatorService::ActuatorService(Stheno* runtime, UUIDPtr& iid) : UserService(runtime, id(), iid), m_actuatorParams(0), m_sap(0) {
}

ActuatorService::ActuatorService(Stheno* runtime, const char* id) :
UserService(runtime, id),
m_actuatorParams(0), m_sap(0) {

}

ActuatorService::ActuatorService(Stheno* runtime, const char* id, UUIDPtr& iid) :
UserService(runtime, id, iid),
m_actuatorParams(0), m_sap(0) {

}

ActuatorService::~ActuatorService() {
}

ServiceAbstract* ActuatorService::duplicate() throw (ServiceException&) {
    return new ActuatorService(m_runtime->duplicate());
}

const char* ActuatorService::getName() {
    return "actuator";
}

const char* ActuatorService::id() {
    return "1212C3F005459F672057661FBE9C70BB";
}

void ActuatorService::getSAP(SAPInfoPtr& sap) {
    sap = m_sap->getSAPInfo();
}

bool ActuatorService::handleActionPacket(ActionPacket* ap) {
    //printf("========================> Action <================\n");
    sendAction(ap);
    return true;
}

void ActuatorService::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    //m_streamParams = static_cast<ServiceStreamParams*>(params);
    list<QoSEndpoint> qosList;
    QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
    QoSEndpoint srt(QoS::SRT, CPUQoS::MED_RT_PRIO);
    QoSEndpoint be(QoS::BE, 0);
    qosList.push_back(hrt);
    qosList.push_back(srt);
    qosList.push_back(be);
    String itf;
    if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i(%d) creating SAP... exception!\n"), m_status));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 2....\n"), m_status));
    m_sap = new ActuatorServiceSAP(this);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 3....\n"), m_status));
    m_sap->open(itf, qosList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ActuatorService::open_i -  creating SAP 3.... %@\n"), params->getQuerySpecializationBlob()));

    m_overlayInterface = m_runtime->getOverlayInterface();

    m_actuatorParams = new ServiceActuatorParams(params.get()->duplicate());


    list<SensorInfoPtr>& sensorList = m_actuatorParams->getSensorList();
    list<SensorInfoPtr>::iterator iter = sensorList.begin();
    while (iter != sensorList.end()) {
        EndpointPtr endpoint = (*iter)->getEndpoint();
        QoSEndpoint qosE = *(endpoint->getQoS());
        ACE_Connector<ActuatorClientHandler, ACE_SOCK_Connector> connector;
        ThreadPerConnection *tpc = new ThreadPerConnection();
        //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
        //      ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
        ExecutionModelPtr* tpcPrt = new
                ExecutionModelPtr(tpc);
        UUIDPtr runtimeUUID;
        UUIDPtr fid;
        m_runtime->getUUID(runtimeUUID);
        m_overlayInterface->getMesh()->getFID(fid);

        ActuatorClientHandler* client = new ActuatorClientHandler(
                runtimeUUID,
                fid,
                qosE,
                false, false, tpcPrt, 0, 0, 0);
        CPUReservation* reserve = 0;
        CPUQoS* cpuQoS = new CPUPriorityQoS(endpoint->getQoS()->getScheduleType(), endpoint->getQoS()->getPriority());
        if(this->getQoSManager()!=0){
           reserve = this->getQoSManager()->createCPUReservation("HRT", cpuQoS);
        }
        //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);        
        (tpc)->bind(client);
        (tpc)->open(reserve, cpuQoS);
        //tpc->bind(client);
        connector.reactor(tpc->getResources()->getReactor());

        if (connector.connect(client, endpoint.get()->getAddr()) == -1) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                    ACE_TEXT("(%T)ERROR: SuperPeerDiscovery::executeQuery() - connect failed:")));
            client->close();
            delete client;
            client = 0;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorService::open_i() - Connect to SENSOR OK!\n")));

        }
        if (m_sensorClients.add(client) == -1) {
        } else {
            if (client->asynchronous(true, false) == -1) {
                client->close();
            }
        }
        iter++;
    }
    return;
}

void ActuatorService::sendAction(ActionPacket* ap) {
    m_sensorClients.sendAction(ap);
}

ServiceClient* ActuatorService::getClient(const UUIDPtr& sid, const UUIDPtr& iid, ClientParamsPtr& cp) {
    ActuatorServiceClient* rpcServiceClient = new ActuatorServiceClient(m_runtime->duplicate(), this);
    if (rpcServiceClient->open(sid, iid, cp) != 0) {
        delete rpcServiceClient;
        return 0;
    }
    return rpcServiceClient;
}

bool ActuatorService::changeToPrimaryRole() {
    return true;
}
