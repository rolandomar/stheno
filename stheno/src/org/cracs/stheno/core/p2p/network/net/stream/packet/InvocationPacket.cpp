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
 * File:   InvocationPacket.cpp
 * Author: rmartins
 * 
 * Created on October 27, 2010, 12:26 PM
 */

#include "InvocationPacket.h"

MessagePacket::MessagePacket() {
}

MessagePacket::MessagePacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

MessagePacket::MessagePacket(
        ULong requestID, 
        ACE_Message_Block* mb) :
SthenoStreamPacket(MESSAGE_PACKET_ID, requestID),m_mb(mb) {
}


MessagePacket::~MessagePacket() {
    if(m_mb != 0){
        delete m_mb;
    }
}

void
MessagePacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}


void
MessagePacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
    outputStream.write_ulong(m_mb->total_length());
    outputStream.write_octet_array_mb(m_mb);
}

void
MessagePacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    UInt size = 0;
    if(m_mb != 0){
        delete m_mb;
    }
    inputStream.read_ulong(size);
    m_mb = new ACE_Message_Block((size_t)size);    
    inputStream.read_octet_array((Octet*)m_mb->base(),size);
    m_mb->wr_ptr(size);
}
