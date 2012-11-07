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
 * File:   StartStreamReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 12:37 PM
 */

#include "StartStreamReplyPacket.h"

StartStreamReplyPacket::StartStreamReplyPacket(){}

StartStreamReplyPacket::StartStreamReplyPacket(InputStream& inputStream){
    this->deserialize(inputStream);
}

StartStreamReplyPacket::StartStreamReplyPacket(
        ULong packetID,
        ULong replyPacketID,
        bool returnStatus) :
SthenoStreamPacket(START_STREAM_REPLY_PACKET, packetID),
m_replyPacketID(replyPacketID),
m_returnStatus(returnStatus)
{
}

StartStreamReplyPacket::~StartStreamReplyPacket() {
}

void
StartStreamReplyPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    printf("StartStreamReplyPacket::serialize 1\n");
    UInt bodySize = getBodySerializationSize();
    printf("StartStreamReplyPacket::serialize 2\n");
    m_packetHeader->setMessageSize(bodySize);
    printf("StartStreamReplyPacket::serialize 3\n");
    serializeHeader(outputStream);
    printf("StartStreamReplyPacket::serialize 4\n");
    serializeBody(outputStream);
    printf("StartStreamReplyPacket::serialize 5\n");
}


ULong StartStreamReplyPacket::getPacketReplyID() {
    return m_replyPacketID;

}


void
StartStreamReplyPacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {    
    outputStream.write_ulong(m_replyPacketID);
    outputStream.write_boolean(m_returnStatus);
}

void
StartStreamReplyPacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    inputStream.read_ulong(m_replyPacketID);
    inputStream.read_boolean(m_returnStatus);
}


