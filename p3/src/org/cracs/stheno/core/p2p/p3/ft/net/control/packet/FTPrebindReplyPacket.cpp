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
 * File:   FTPrebindReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on December 2, 2010, 5:12 PM
 */

#include "FTPrebindReplyPacket.h"

FTPrebindReplyPacket::FTPrebindReplyPacket() : m_packetIDReply(0) {
}

FTPrebindReplyPacket::FTPrebindReplyPacket(InputStream& inputStream) :
m_packetIDReply(0) {
    this->deserialize(inputStream);
}

FTPrebindReplyPacket::FTPrebindReplyPacket(
        ULong packetID, UInt packetIDReply, bool acceptance) :
SthenoStreamPacket(PREBIND_REPLY_PACKET_OP, packetID),
m_acceptance(acceptance), m_packetIDReply(packetIDReply) {
}

FTPrebindReplyPacket::~FTPrebindReplyPacket() {
}

void FTPrebindReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool FTPrebindReplyPacket::getAcceptanceValue() {
    return m_acceptance;
}

ULong FTPrebindReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTPrebindReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    outputStream.write_boolean(m_acceptance);
}

void
FTPrebindReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);    
    inputStream.read_boolean(m_acceptance);
}

