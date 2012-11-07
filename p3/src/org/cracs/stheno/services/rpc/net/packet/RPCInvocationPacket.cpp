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
 * File:   RPCInvocationPacket.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:55 PM
 */

#include "RPCInvocationPacket.h"

RPCInvocationPacket::RPCInvocationPacket():
m_payload(0)
{
}

RPCInvocationPacket::RPCInvocationPacket(InputStream& inputStream):
m_payload(0)
{
    this->deserialize(inputStream);
}

RPCInvocationPacket::RPCInvocationPacket(
        ULong requestID,
        UUIDPtr& oid,
        ACE_Message_Block* payload,
        ACE_Time_Value& timestamp
        ) :
SthenoStreamPacket(RPC_INVOCATION_PACKET_OP, requestID),
m_oid(oid), m_payload(payload),m_timestamp(timestamp) {
}

RPCInvocationPacket::~RPCInvocationPacket() {
    ACE_Message_Block::release(m_payload);
}

void
RPCInvocationPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ACE_Message_Block* RPCInvocationPacket::getPayload() {
    return m_payload;
}

/*ACE_Time_Value& RPCInvocationPacket::getTimestamp(){
    return m_timestamp;    
}*/

void
RPCInvocationPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {    
    m_oid->serialize(outputStream);
        
    UInt size = m_payload->total_length();
    outputStream.write_ulong(size);
    outputStream.write_octet_array_mb(m_payload);
    
    //outputStream.write_longlong(m_timestamp.sec());
    //outputStream.write_ulonglong(m_timestamp.usec());
}

void
RPCInvocationPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* oid = new UUID(inputStream);
    m_oid.reset(oid);
    
    UInt size = 0;
    inputStream.read_ulong(size);
    ACE_Message_Block::release(m_payload);
    m_payload = 0;
    m_payload = new ACE_Message_Block((size_t) size);
    inputStream.read_octet_array((Octet*) m_payload->base(), size);
    m_payload->wr_ptr(size);

    //LongLong secs;
    //ULongLong usecs;
    //inputStream.read_longlong(secs);
    //inputStream.read_ulonglong(usecs);
    //m_timestamp.set(secs,usecs);

}

UUIDPtr& RPCInvocationPacket::getOID() {
    return m_oid;
}



