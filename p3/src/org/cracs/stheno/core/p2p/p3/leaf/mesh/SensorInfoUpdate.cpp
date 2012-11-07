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
 * File:   SensorInfoUpdate.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 4:45 PM
 */

#include "SensorInfoUpdate.h"
#include "UUID.h"

SensorInfoUpdate::SensorInfoUpdate(int op, UUIDPtr& uuid, EndpointPtr& e) :
InfoUpdate(SENSOR_UPDATE_TYPE, op),
m_endpoint(e),
m_uuid(uuid) {
}

SensorInfoUpdate::SensorInfoUpdate(InfoUpdate* info, InputStream& inputStream) :
InfoUpdate(*info) {
    delete info;
    this->deserializeSpecializationBody(inputStream);
}

SensorInfoUpdate::SensorInfoUpdate(InputStream& inputStream) : InfoUpdate() {
    deserialize(inputStream);
}

SensorInfoUpdate::~SensorInfoUpdate() {
}

void SensorInfoUpdate::serialize(OutputStream& outputStream) throw (SerializationException&) {
    InfoUpdate::serialize(outputStream);
    serializeSpecializationBody(outputStream);
}

void SensorInfoUpdate::deserialize(InputStream& inputStream) throw (SerializationException&) {
    InfoUpdate::deserialize(inputStream);
    deserializeSpecializationBody(inputStream);
}

void SensorInfoUpdate::serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_endpoint->serialize(outputStream);
}

void SensorInfoUpdate::deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    Endpoint* e = new Endpoint(inputStream);
    m_endpoint.reset(e);
}

UUIDPtr& SensorInfoUpdate::getUUID() {
    return m_uuid;
}

EndpointPtr& SensorInfoUpdate::getEndpoint() {
    return m_endpoint;
}