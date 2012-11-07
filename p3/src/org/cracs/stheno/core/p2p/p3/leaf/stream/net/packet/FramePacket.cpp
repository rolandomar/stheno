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
 * File:   FramePacket.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 3:15 PM
 */

#include "FramePacket.h"

FramePacket::FramePacket() :
m_payload(0) {
}

FramePacket::FramePacket(const FramePacket& orig) :
m_frameSize(orig.m_frameSize), m_payload(orig.m_payload->duplicate()), m_frameNo(orig.m_frameNo) {
}

FramePacket::FramePacket(InputStream& inputStream) :
m_payload(0) {
    this->deserialize(inputStream);
}

FramePacket::FramePacket(
        ULong requestID,
        ULong frameNo,
        int frameSize,
        ACE_Message_Block* payload
        ) :
SthenoStreamPacket(FRAME_PACKET_OP, requestID),
m_frameSize(frameSize), m_payload(payload), m_frameNo(frameNo) {
}

FramePacket::~FramePacket() {
      ACE_Message_Block::release(m_payload);
}

FramePacket* FramePacket::duplicate() {
    return new FramePacket(*this);
}

void
FramePacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* FramePacket::getPayload() {
    return m_payload;
}

ACE_Time_Value& FramePacket::getTimestamp(){
    return m_timestamp;
}

void
FramePacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_frameNo);
    outputStream.write_long(m_frameSize);
    
    m_timestamp = ACE_OS::gettimeofday();
    outputStream.write_longlong(m_timestamp.sec());
    outputStream.write_ulonglong(m_timestamp.usec());
    
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);

}

void
FramePacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_frameNo);
    inputStream.read_long(m_frameSize);
    
    LongLong secs;
    ULongLong usecs;
    inputStream.read_longlong(secs);
    inputStream.read_ulonglong(usecs);
    m_timestamp.set(secs,usecs);
    
    UInt size = 0;
    inputStream.read_ulong(size);
    if (m_payload != 0) {
        ACE_Message_Block::release(m_payload);                        
    }
    m_payload = new ACE_Message_Block((size_t) size);
    inputStream.read_octet_array((Octet*) m_payload->base(), size);
    m_payload->wr_ptr(size);

}

int FramePacket::getFrameSize() {
    return m_frameSize;
}

ULong FramePacket::getFrameNo() {
    return m_frameNo;
}