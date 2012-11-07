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
 * File:   FTDataReplicationPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 5:26 PM
 */

#include "FTDataReplicationPacket.h"

FTDataReplicationPacket::FTDataReplicationPacket() : m_payload(0) {
    //printf("FTDataReplicationPacket::FTDataReplicationPacket()\n");
}

FTDataReplicationPacket::FTDataReplicationPacket(InputStream& inputStream) :
m_payload(0) {
    //printf("FTDataReplicationPacket::FTDataReplicationPacket(InputStream& inputStream)\n");
    this->deserialize(inputStream);
}

FTDataReplicationPacket::FTDataReplicationPacket(
        ULong requestID,
        ACE_Message_Block* payload) :
SthenoStreamPacket(DATA_REPLICATION_PACKET_OP, requestID),
m_payload(payload) {
    //printf("FTDataReplicationPacket::FTDataReplicationPacket( ULong requestID,ACE_Message_Block* payload)\n");
}

FTDataReplicationPacket::~FTDataReplicationPacket() {
    //printf("FTDataReplicationPacket::~FTDataReplicationPacket()\n");
    ACE_Message_Block::release(m_payload);
}

void
FTDataReplicationPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* FTDataReplicationPacket::getPayload() {
    return m_payload;
}

void
FTDataReplicationPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);
}

void
FTDataReplicationPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UInt size = 0;
    inputStream.read_ulong(size);
    ACE_Message_Block::release(m_payload);
    if (size > 0) {
        m_payload = new ACE_Message_Block((size_t) size);
        inputStream.read_octet_array((Octet*) m_payload->base(), size);
        m_payload->wr_ptr(size);
    }
}