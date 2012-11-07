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
 * File:   ResumeStreamServicePacket.cpp
 * Author: rmartins
 * 
 * Created on December 17, 2010, 12:07 PM
 */

#include "ResumeStreamServicePacket.h"

ResumeStreamServicePacket::ResumeStreamServicePacket() {
}

ResumeStreamServicePacket::ResumeStreamServicePacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

ResumeStreamServicePacket::ResumeStreamServicePacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        UInt lastFrameNo
        ) :
SthenoStreamPacket(RESUME_STREAM_SERVICE_PACKET_OP, requestID),
m_uuid(uuid),
m_fid(fid),
m_lastFrameNo(lastFrameNo) {
}

ResumeStreamServicePacket::~ResumeStreamServicePacket() {
}

void
ResumeStreamServicePacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& ResumeStreamServicePacket::getUUID() {
    return m_uuid;
}

UUIDPtr& ResumeStreamServicePacket::getFID() {
    return m_fid;
}

UInt ResumeStreamServicePacket::getLastFrameNo(){
    return m_lastFrameNo;
}

void
ResumeStreamServicePacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    outputStream.write_ulong(m_lastFrameNo);
}

void
ResumeStreamServicePacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    inputStream.read_ulong(m_lastFrameNo);
}


