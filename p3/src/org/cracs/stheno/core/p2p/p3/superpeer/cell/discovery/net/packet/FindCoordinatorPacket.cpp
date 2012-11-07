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
 * File:   FindCoordinatorPacket.cpp
 * Author: rmartins
 * 
 * Created on August 24, 2010, 2:19 PM
 */

#include "FindCoordinatorPacket.h"

FindCoordinatorPacket::~FindCoordinatorPacket() {
}

FindCoordinatorPacket::FindCoordinatorPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, FIND_COORDINATOR_PACKET, requestID) {

}
//RequestCellPacket(){}

FindCoordinatorPacket::FindCoordinatorPacket() :
SthenoPacket(0) {
}

void
FindCoordinatorPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized header!len=%lu\n", outputStream.total_length());
    serializeBody(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized body!len=%lu\n", outputStream.total_length());
}

void
FindCoordinatorPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
}

void
FindCoordinatorPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
}