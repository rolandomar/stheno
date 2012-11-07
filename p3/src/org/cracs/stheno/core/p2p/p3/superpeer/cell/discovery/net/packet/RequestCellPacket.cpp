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
 * File:   RequestCellPacket.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 27, 2010, 11:21 AM
 */

#include "RequestCellPacket.h"

RequestCellPacket::~RequestCellPacket() {
}

RequestCellPacket::RequestCellPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, REQUEST_CELL_PACKET, requestID) {

}

RequestCellPacket::RequestCellPacket() :
SthenoPacket(0) {
}

void RequestCellPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void
RequestCellPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
}

void
RequestCellPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
}
