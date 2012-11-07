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
 * File:   FTDataPrebindPacket.cpp
 * Author: rmartins
 * 
 * Created on December 3, 2010, 11:34 AM
 */

#include "FTDataPrebindPacket.h"


FTDataPrebindPacket::FTDataPrebindPacket(){

}

FTDataPrebindPacket::FTDataPrebindPacket(InputStream& inputStream){
    this->deserialize(inputStream);
}

FTDataPrebindPacket::FTDataPrebindPacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        UUIDPtr& rgid
        ) :
SthenoStreamPacket(FT_DATA_PREBIND_PACKET_OP, requestID),
m_uuid(uuid),
m_fid(fid),
m_rgid(rgid)
{
}

FTDataPrebindPacket::~FTDataPrebindPacket() {
}

void
FTDataPrebindPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& FTDataPrebindPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& FTDataPrebindPacket::getFID() {
    return m_fid;
}

UUIDPtr& FTDataPrebindPacket::getRGID() {
    return m_rgid;
}

void
FTDataPrebindPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {    
    m_uuid->serialize(outputStream);    
    m_fid->serialize(outputStream);
    m_rgid->serialize(outputStream);
}

void
FTDataPrebindPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    UUID* rgid = new UUID(inputStream);
    m_rgid.reset(rgid);
}

