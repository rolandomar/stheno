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

#include "ActionPacket.h"

ActionPacket::ActionPacket():
m_payload(0)
{
}

ActionPacket::ActionPacket(InputStream& inputStream):
m_payload(0)
{
    this->deserialize(inputStream);
}

ActionPacket::ActionPacket(
        ULong requestID,
        ACE_Time_Value& timestamp,
        ACE_Message_Block* payload
        ) :
SthenoStreamPacket(ACTION_PACKET_OP, requestID),
m_payload(payload),m_timestamp(timestamp) {
}

ActionPacket::~ActionPacket() {
    ACE_Message_Block::release(m_payload);
}

void
ActionPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* ActionPacket::getPayload() {
    return m_payload;
}

ACE_Time_Value& ActionPacket::getTimestamp(){
    return m_timestamp;    
}

void
ActionPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    //we are relaying, FramePacket is the original source of then timestamp
    /*m_timestamp = ACE_OS::gettimeofday();*/
    outputStream.write_longlong(m_timestamp.sec());
    outputStream.write_ulonglong(m_timestamp.usec());
    
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);

}

void
ActionPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
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
