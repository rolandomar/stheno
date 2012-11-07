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
 * File:   StartStreamPacket.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:50 PM
 */

#include "StartStreamServicePacket.h"

StartStreamServicePacket::StartStreamServicePacket() {
}

StartStreamServicePacket::StartStreamServicePacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

StartStreamServicePacket::StartStreamServicePacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid
        ) :
SthenoStreamPacket(START_STREAM_SERVICE_PACKET_OP, requestID), m_uuid(uuid), m_fid(fid){
}

StartStreamServicePacket::~StartStreamServicePacket() {
}

void
StartStreamServicePacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& StartStreamServicePacket::getUUID() {
    return m_uuid;
}

UUIDPtr& StartStreamServicePacket::getFID() {
    return m_fid;
}

void
StartStreamServicePacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
}

void
StartStreamServicePacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
}

