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
 * File:   StreamMissingFramePacket.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 3:27 PM
 */

#include "StreamMissingFramePacket.h"

StreamMissingFramePacket::StreamMissingFramePacket() {
}

StreamMissingFramePacket::StreamMissingFramePacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

StreamMissingFramePacket::StreamMissingFramePacket(
        ULong requestID, ULong missingFrameID
        ) :
SthenoStreamPacket(STREAM_MISSING_FRAME_PACKET_OP, requestID),
m_missingFrameID(missingFrameID) {
}

StreamMissingFramePacket::~StreamMissingFramePacket() {
}

void
StreamMissingFramePacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void
StreamMissingFramePacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_long(m_missingFrameID);
}

void
StreamMissingFramePacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_long(m_missingFrameID);
}

int StreamMissingFramePacket::getMissingFrameID() {
    return m_missingFrameID;
}

