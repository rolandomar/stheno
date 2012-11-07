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
 * File:   SensorInfo.cpp
 * Author: rmartins
 * 
 * Created on November 19, 2010, 2:36 PM
 */

#include "SensorInfo.h"

SensorInfo::SensorInfo(int type, UUIDPtr& uuid, EndpointPtr& e) :
m_uuid(uuid), m_endpoint(e), m_type(type) {
}

SensorInfo::SensorInfo(InputStream& inputStream) {
    this->deserialize(inputStream);
}

SensorInfo::SensorInfo(const SensorInfo& orig) :
m_uuid(orig.m_uuid), m_endpoint(orig.m_endpoint), m_type(orig.m_type) {
}

SensorInfo::~SensorInfo() {
}

void SensorInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_long(m_type);
    m_uuid->serialize(outputStream);
    m_endpoint->serialize(outputStream);
}

void SensorInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_long(m_type);
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    Endpoint* e = new Endpoint(inputStream);
    m_endpoint.reset(e);
}

int SensorInfo::getType() {
    return m_type;
}

UUIDPtr& SensorInfo::getUUID() {
    return m_uuid;
}

EndpointPtr& SensorInfo::getEndpoint() {
    return m_endpoint;
}
