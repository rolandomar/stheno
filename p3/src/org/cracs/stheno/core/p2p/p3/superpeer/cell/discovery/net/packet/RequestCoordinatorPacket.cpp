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
 * File:   RequestCoordinatorPacket.cpp
 * Author: rmartins
 * 
 * Created on September 15, 2010, 2:47 PM
 */

#include "RequestCoordinatorPacket.h"

RequestCoordinatorPacket::~RequestCoordinatorPacket() {
}

RequestCoordinatorPacket::RequestCoordinatorPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, REQUEST_COORDINATOR_PACKET, requestID) {

}
//RequestCellPacket(){}

RequestCoordinatorPacket::RequestCoordinatorPacket() :
SthenoPacket(0) {
}

void
RequestCoordinatorPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized header!len=%lu\n", outputStream.total_length());
    serializeBody(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized body!len=%lu\n", outputStream.total_length());
}

void
RequestCoordinatorPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
}

void
RequestCoordinatorPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
}