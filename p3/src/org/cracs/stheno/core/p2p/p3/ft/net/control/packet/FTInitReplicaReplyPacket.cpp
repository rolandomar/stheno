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
 * File:   FTInitReplicaReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 6:32 PM
 */

#include "FTInitReplicaReplyPacket.h"


FTInitReplicaReplyPacket::FTInitReplicaReplyPacket() : m_packetIDReply(0) {
}

FTInitReplicaReplyPacket::FTInitReplicaReplyPacket(InputStream& inputStream) :
m_packetIDReply(0) {
    this->deserialize(inputStream);
}

FTInitReplicaReplyPacket::FTInitReplicaReplyPacket(
        ULong packetID, UInt packetIDReply, SAPInfoPtr& ftDataSAPInfo) :
SthenoStreamPacket(INIT_REPLICA_REPLY_PACKET_OP, packetID),
m_ftDataSAPInfo(ftDataSAPInfo), m_packetIDReply(packetIDReply) {
}

FTInitReplicaReplyPacket::~FTInitReplicaReplyPacket() {
}

void FTInitReplicaReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

SAPInfoPtr& FTInitReplicaReplyPacket::getFTDataSAPInfo() {
    return m_ftDataSAPInfo;
}

ULong FTInitReplicaReplyPacket::getPacketReplyID() {
    return m_packetIDReply;
}

void
FTInitReplicaReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    outputStream.write_ulong(m_packetIDReply);
    if(!m_ftDataSAPInfo.null()){
        outputStream.write_boolean(true);
        m_ftDataSAPInfo->serialize(outputStream);
    }else{
        outputStream.write_boolean(false);
    }
}

void
FTInitReplicaReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    inputStream.read_ulong(m_packetIDReply);
    Boolean flag;
    inputStream.read_boolean(flag);
    if(flag){
        SAPInfo* info = new SAPInfo(inputStream);
        m_ftDataSAPInfo.reset(info);
    }else{
        m_ftDataSAPInfo.reset(0);
    }
}