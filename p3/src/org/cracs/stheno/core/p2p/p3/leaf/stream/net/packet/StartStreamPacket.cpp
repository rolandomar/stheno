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

#include "StartStreamPacket.h"

StartStreamPacket::StartStreamPacket() {
}

StartStreamPacket::StartStreamPacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

StartStreamPacket::StartStreamPacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        int frameSize,
        int frameRate
        ) :
SthenoStreamPacket(START_STREAM_PACKET_OP, requestID), m_uuid(uuid), m_fid(fid),
m_frameSize(frameSize), m_frameRate(frameRate) {
}

StartStreamPacket::~StartStreamPacket() {
}

void
StartStreamPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& StartStreamPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& StartStreamPacket::getFID() {
    return m_fid;
}

void
StartStreamPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    outputStream.write_long(m_frameSize);
    outputStream.write_long(m_frameRate);
}

void
StartStreamPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    inputStream.read_long(m_frameSize);
    inputStream.read_long(m_frameRate);    
}

int StartStreamPacket::getFrameSize() {
    return m_frameSize;
}

int StartStreamPacket::getFrameRate() {
    return m_frameRate;
}
