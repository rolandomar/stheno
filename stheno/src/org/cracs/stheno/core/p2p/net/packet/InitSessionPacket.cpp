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
 * File:   InitSessionPacket.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 1:22 PM
 */

#include "InitSessionPacket.h"

InitSessionPacket::InitSessionPacket() {
}

InitSessionPacket::InitSessionPacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

InitSessionPacket::InitSessionPacket(ULong requestID, UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qos) :
SthenoStreamPacket(INIT_SESSION_PACKET_OP, requestID), m_uuid(uuid), m_fid(fid), m_qos(qos) {
}

InitSessionPacket::~InitSessionPacket() {
}

void
InitSessionPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& InitSessionPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& InitSessionPacket::getFID() {
    return m_fid;
}

QoSEndpoint& InitSessionPacket::getQoS() {
    return m_qos;
}

void
InitSessionPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_qos.serialize(outputStream);
}

void
InitSessionPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    m_qos.deserialize(inputStream);

}


