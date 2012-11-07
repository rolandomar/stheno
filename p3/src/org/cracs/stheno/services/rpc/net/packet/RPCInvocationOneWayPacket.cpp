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
 * File:   RPCInvocationOneWayPacket.cpp
 * Author: rmartins
 * 
 * Created on January 22, 2011, 8:01 PM
 */

#include "RPCInvocationOneWayPacket.h"

RPCInvocationOneWayPacket::RPCInvocationOneWayPacket():
m_payload(0)
{
}

RPCInvocationOneWayPacket::RPCInvocationOneWayPacket(InputStream& inputStream):
m_payload(0)
{
    this->deserialize(inputStream);
}

RPCInvocationOneWayPacket::RPCInvocationOneWayPacket(
        ULong requestID,
        UUIDPtr& oid,
        ACE_Message_Block* payload,
        ACE_Time_Value& timestamp
        ) :
SthenoStreamPacket(RPC_ONE_WAY_INVOCATION_PACKET_OP, requestID),
m_oid(oid), m_payload(payload),m_timestamp(timestamp) {
}

RPCInvocationOneWayPacket::~RPCInvocationOneWayPacket() {
    ACE_Message_Block::release(m_payload);
}

void
RPCInvocationOneWayPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* RPCInvocationOneWayPacket::getPayload() {
    return m_payload;
}

ACE_Time_Value& RPCInvocationOneWayPacket::getTimestamp(){
    return m_timestamp;    
}

void
RPCInvocationOneWayPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {    
    m_oid->serialize(outputStream);
        
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);
    
    outputStream.write_longlong(m_timestamp.sec());
    outputStream.write_ulonglong(m_timestamp.usec());
}

void
RPCInvocationOneWayPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* oid = new UUID(inputStream);
    m_oid.reset(oid);
    
    UInt size = 0;
    inputStream.read_ulong(size);
    ACE_Message_Block::release(m_payload);
    m_payload = new ACE_Message_Block((size_t) size);
    inputStream.read_octet_array((Octet*) m_payload->base(), size);
    m_payload->wr_ptr(size);

    LongLong secs;
    ULongLong usecs;
    inputStream.read_longlong(secs);
    inputStream.read_ulonglong(usecs);
    m_timestamp.set(secs,usecs);

}

UUIDPtr& RPCInvocationOneWayPacket::getOID() {
    return m_oid;
}
