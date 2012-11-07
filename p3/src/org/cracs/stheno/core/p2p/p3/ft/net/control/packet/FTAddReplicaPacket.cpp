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
 * File:   FTAddReplicaPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 1:13 PM
 */

#include "FTAddReplicaPacket.h"

FTAddReplicaPacket::FTAddReplicaPacket() {
}

FTAddReplicaPacket::FTAddReplicaPacket(InputStream& inputStream)
{
    this->deserialize(inputStream);
}

FTAddReplicaPacket::FTAddReplicaPacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        UUIDPtr& rgid,
        OverlayPeerInfoPtr& peer
        ) :
SthenoStreamPacket(ADD_REPLICA_PACKET_OP, requestID),
m_uuid(uuid),
m_fid(fid),
m_rgid(rgid),
m_peer(peer){
}

FTAddReplicaPacket::~FTAddReplicaPacket() {
}

void
FTAddReplicaPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& FTAddReplicaPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& FTAddReplicaPacket::getFID() {
    return m_fid;
}

UUIDPtr& FTAddReplicaPacket::getRGID() {
    return m_rgid;
}

OverlayPeerInfoPtr& FTAddReplicaPacket::getPeerInfo(){
    return m_peer;
}


void
FTAddReplicaPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_rgid->serialize(outputStream);    
    m_peer->serialize(outputStream);    
}

void
FTAddReplicaPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    UUID* rgid = new UUID(inputStream);
    m_rgid.reset(rgid);
    OverlayPeerInfo* pi = new OverlayPeerInfo(inputStream);
    m_peer.reset(pi);
}