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
 * File:   ServiceActuatorParams.cpp
 * Author: rmartins
 * 
 * Created on October 26, 2010, 12:13 PM
 */

#include "ServiceActuatorParams.h"
#include "ActuatorQoS.h"

ServiceActuatorParams::ServiceActuatorParams(
        const UUIDPtr& sid,
        const list<SensorInfoPtr>& sensorList,        
        ActuatorQoS* qos
        ) :
ServiceParams(sid),
m_sensorList(sensorList),
m_qos(qos) {
}

ServiceActuatorParams::ServiceActuatorParams(
        const UUIDPtr& sid,
        const list<SensorInfoPtr>& sensorList,        
        ActuatorQoS* qos,
        FTServiceParamsPtr& ftParams) :
ServiceParams(sid, ftParams),
m_sensorList(sensorList),
m_qos(qos) {
}

ServiceActuatorParams::ServiceActuatorParams(ServiceParams* params):
m_qos(0),ServiceParams(params)
{
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}


ServiceActuatorParams::~ServiceActuatorParams() {
    if(m_qos != 0){
        delete m_qos;
    }
}

list<SensorInfoPtr>& ServiceActuatorParams::getSensorList() {
    return m_sensorList;
}


ActuatorQoS* ServiceActuatorParams::getQoS() {
    return m_qos;
}
