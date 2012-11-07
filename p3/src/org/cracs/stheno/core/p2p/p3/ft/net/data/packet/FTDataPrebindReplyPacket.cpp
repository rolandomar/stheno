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
 * File:   FTDataPrebindReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on December 3, 2010, 11:34 AM
 */

#include "FTDataPrebindReplyPacket.h"


FTDataPrebindReplyPacket::FTDataPrebindReplyPacket() : m_packetIDReply(0) {
}

FTDataPrebindReplyPacket::FTDataPrebindReplyPacket(InputStream& inputStream) :
m_packetIDReply(0) {
    this->deserialize(inputStream);
}

FTDataPrebindReplyPacket::FTDataPrebindReplyPacket(
        ULong packetID, UInt packetIDReply, bool acceptance) :
SthenoStreamPacket(FT_DATA_PREBIND_REPLY_PACKET_OP, packetID),
m_acceptance(acceptance), m_packetIDReply(packetIDReply) {
}

FTDataPrebindReplyPacket::~FTDataPrebindReplyPacket() {
}

void FTDataPrebindReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool FTDataPrebindReplyPacket::getAcceptanceValue() {
    return m_acceptance;
}

ULong FTDataPrebindReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTDataPrebindReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    outputStream.write_boolean(m_acceptance);
}

void
FTDataPrebindReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);    
    inputStream.read_boolean(m_acceptance);
}

