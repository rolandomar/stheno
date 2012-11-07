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

#include "StreamFramePacket.h"

StreamFramePacket::StreamFramePacket():
m_payload(0)
{
}

StreamFramePacket::StreamFramePacket(InputStream& inputStream):
m_payload(0)
{
    this->deserialize(inputStream);
}

StreamFramePacket::StreamFramePacket(
        ULong requestID,
        ULong frameNo,
        int frameSize,
        ACE_Time_Value& timestamp,
        ACE_Message_Block* payload
        ) :
SthenoStreamPacket(STREAM_FRAME_PACKET_OP, requestID),
m_frameNo(frameNo),m_frameSize(frameSize), m_payload(payload),m_timestamp(timestamp) {
}

StreamFramePacket::~StreamFramePacket() {
    ACE_Message_Block::release(m_payload);
}

void
StreamFramePacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* StreamFramePacket::getPayload() {
    return m_payload;
}

ACE_Time_Value& StreamFramePacket::getTimestamp(){
    return m_timestamp;    
}

void
StreamFramePacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_frameNo);
    outputStream.write_long(m_frameSize);
    
    //we are relaying, FramePacket is the original source of then timestamp
    /*m_timestamp = ACE_OS::gettimeofday();*/
    outputStream.write_longlong(m_timestamp.sec());
    outputStream.write_ulonglong(m_timestamp.usec());
    
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);

}

void
StreamFramePacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_frameNo);
    inputStream.read_long(m_frameSize);
    LongLong secs;
    ULongLong usecs;
    inputStream.read_longlong(secs);
    inputStream.read_ulonglong(usecs);
    m_timestamp.set(secs,usecs);
    UInt size = 0;
    inputStream.read_ulong(size);
    ACE_Message_Block::release(m_payload);
    m_payload = new ACE_Message_Block((size_t) size);
    inputStream.read_octet_array((Octet*) m_payload->base(), size);
    m_payload->wr_ptr(size);

}

int StreamFramePacket::getFrameSize() {
    return m_frameSize;
}

ULong StreamFramePacket::getFrameNo(){
    return m_frameNo;
}
