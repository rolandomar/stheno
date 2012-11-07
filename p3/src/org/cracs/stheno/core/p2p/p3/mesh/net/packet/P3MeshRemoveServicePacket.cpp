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
 * File:   P3MeshRemoveServicePacket.cpp
 * Author: rmartins
 * 
 * Created on October 1, 2010, 9:48 AM
 */

#include "P3MeshRemoveServicePacket.h"

P3MeshRemoveServicePacket::P3MeshRemoveServicePacket(){
    
}

P3MeshRemoveServicePacket::P3MeshRemoveServicePacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

P3MeshRemoveServicePacket::P3MeshRemoveServicePacket(        
        UUIDPtr sid,UUIDPtr iid) :
SthenoStreamPacket(P3MESH_REMOVESERVICE_PACKET, 0), m_sid(sid),m_iid(iid) {
}

P3MeshRemoveServicePacket::~P3MeshRemoveServicePacket() {
}

void
P3MeshRemoveServicePacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& P3MeshRemoveServicePacket::getSID(){
    return m_sid;
}

UUIDPtr& P3MeshRemoveServicePacket::getIID(){
    return m_iid;
}

void
P3MeshRemoveServicePacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
   m_sid->serialize(outputStream);
   m_iid->serialize(outputStream);
}

void
P3MeshRemoveServicePacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* iid = new UUID(inputStream);
    m_iid.reset(iid);
}

