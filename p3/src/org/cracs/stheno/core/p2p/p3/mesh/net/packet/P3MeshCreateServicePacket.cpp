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
 * File:   P3MeshCreateServicePacket.cpp
 * Author: rmartins
 * 
 * Created on October 1, 2010, 9:47 AM
 */

#include "P3MeshCreateServicePacket.h"

P3MeshCreateServicePacket::P3MeshCreateServicePacket():m_params(0){
    
}

P3MeshCreateServicePacket::P3MeshCreateServicePacket(InputStream& inputStream):
m_params(0)
{
    this->deserialize(inputStream);
}

P3MeshCreateServicePacket::P3MeshCreateServicePacket(        
        ServiceParamsPtr& params) :
SthenoStreamPacket(P3MESH_CREATESERVICE_PACKET, 0), m_params(params) {
}

P3MeshCreateServicePacket::~P3MeshCreateServicePacket() {
}

void
P3MeshCreateServicePacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ServiceParamsPtr& P3MeshCreateServicePacket::getServiceParams(){
    return m_params;
}

void
P3MeshCreateServicePacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
    if(m_params.null()){
        throw SerializationException(SerializationException::NULL_OBJECT);
    }
    m_params->serialize(outputStream);
}

void
P3MeshCreateServicePacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {    
    ServiceParams* params = new ServiceParams(inputStream);    
    m_params.reset(params);
}


