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
 * File:   FTDataReplicationReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 5:26 PM
 */

#include "FTDataReplicationReplyPacket.h"

FTDataReplicationReplyPacket::FTDataReplicationReplyPacket() : m_acception(0), m_packetIDReply(0) {
    //printf("FTDataReplicationReplyPacket::FTDataReplicationReplyPacket()\n");
}

FTDataReplicationReplyPacket::FTDataReplicationReplyPacket(InputStream& inputStream) :
m_acception(0), m_packetIDReply(0) {
    //printf("FTDataReplicationReplyPacket::FTDataReplicationReplyPacket(InputStream& inputStream)\n");
    this->deserialize(inputStream);
}

FTDataReplicationReplyPacket::FTDataReplicationReplyPacket(
        ULong packetID, UInt packetIDReply, bool acception) :
SthenoStreamPacket(DATA_REPLICATION_REPLY_PACKET_OP, packetID),
m_acception(acception), m_packetIDReply(packetIDReply) {
    //printf("FTDataReplicationReplyPacket::FTDataReplicationReplyPacket(ULong packetID, UInt packetIDReply, bool acception)\n");
}

FTDataReplicationReplyPacket::~FTDataReplicationReplyPacket() {
    //printf("FTDataReplicationReplyPacket::~FTDataReplicationReplyPacket()\n");
}

void FTDataReplicationReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool FTDataReplicationReplyPacket::getJoinResult() {
    return m_acception;
}

ULong FTDataReplicationReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTDataReplicationReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    outputStream.write_boolean(m_acception);
}

void
FTDataReplicationReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);
    inputStream.read_boolean(m_acception);
}