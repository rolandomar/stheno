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
 * File:   FTInitDataGroupReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 6:34 PM
 */

#include "FTInitDataGroupReplyPacket.h"


FTInitDataGroupReplyPacket::FTInitDataGroupReplyPacket() : m_acception(0), m_packetIDReply(0) {
}

FTInitDataGroupReplyPacket::FTInitDataGroupReplyPacket(InputStream& inputStream) :
m_acception(0), m_packetIDReply(0) {
    this->deserialize(inputStream);
}

FTInitDataGroupReplyPacket::FTInitDataGroupReplyPacket(
        ULong packetID, UInt packetIDReply, bool acception) :
SthenoStreamPacket(INIT_DATA_GROUP_REPLY_PACKET_OP, packetID),
m_acception(acception), m_packetIDReply(packetIDReply) {
}

FTInitDataGroupReplyPacket::~FTInitDataGroupReplyPacket() {
}

void FTInitDataGroupReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool FTInitDataGroupReplyPacket::getJoinResult() {
    return m_acception;
}

ULong FTInitDataGroupReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTInitDataGroupReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    outputStream.write_boolean(m_acception);
}

void
FTInitDataGroupReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);
    inputStream.read_boolean(m_acception);
}