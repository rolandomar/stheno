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
 * File:   FTRemoveReplicaReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 6:30 PM
 */

#include "FTRemoveReplicaReplyPacket.h"


FTRemoveReplicaReplyPacket::FTRemoveReplicaReplyPacket() : m_acception(0), m_packetIDReply(0) {
}

FTRemoveReplicaReplyPacket::FTRemoveReplicaReplyPacket(InputStream& inputStream) :
m_acception(0), m_packetIDReply(0) {
    this->deserialize(inputStream);
}

FTRemoveReplicaReplyPacket::FTRemoveReplicaReplyPacket(
        ULong packetID, UInt packetIDReply, bool acception) :
SthenoStreamPacket(REMOVE_REPLICA_REPLY_PACKET_OP, packetID),
m_acception(acception), m_packetIDReply(packetIDReply) {
}

FTRemoveReplicaReplyPacket::~FTRemoveReplicaReplyPacket() {
}

void FTRemoveReplicaReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool FTRemoveReplicaReplyPacket::getJoinResult() {
    return m_acception;
}

ULong FTRemoveReplicaReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTRemoveReplicaReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    outputStream.write_boolean(m_acception);
}

void
FTRemoveReplicaReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);
    inputStream.read_boolean(m_acception);
}