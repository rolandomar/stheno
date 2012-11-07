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
 * File:   ServiceStreamParams.cpp
 * Author: rmartins
 * 
 * Created on October 26, 2010, 12:13 PM
 */

#include "ServiceStreamParams.h"
#include "StreamQoS.h"

ServiceStreamParams::ServiceStreamParams(
        const UUIDPtr& sid,
        const UUIDPtr& sensorUUID,
        EndpointPtr& endpoint,
        StreamQoS* qos
        ) :
ServiceParams(sid),
m_sensorUUID(sensorUUID),
m_endpoint(endpoint),
m_qos(qos) {
}

ServiceStreamParams::ServiceStreamParams(
        const UUIDPtr& sid,
        const UUIDPtr& sensorUUID,
        EndpointPtr& endpoint,
        StreamQoS* qos,
        FTServiceParamsPtr& ftParams) :
ServiceParams(sid, ftParams),
m_sensorUUID(sensorUUID),
m_endpoint(endpoint),
m_qos(qos) {
}

ServiceStreamParams::ServiceStreamParams(ServiceParams* params):
m_qos(0),ServiceParams(params)
{
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}


ServiceStreamParams::~ServiceStreamParams() {
    if(m_qos != 0){
        delete m_qos;
    }
}

UUIDPtr& ServiceStreamParams::getSensorUUID() {
    return m_sensorUUID;
}

EndpointPtr& ServiceStreamParams::getEndpoint() {
    return m_endpoint;
}

StreamQoS* ServiceStreamParams::getQoS() {
    return m_qos;
}
