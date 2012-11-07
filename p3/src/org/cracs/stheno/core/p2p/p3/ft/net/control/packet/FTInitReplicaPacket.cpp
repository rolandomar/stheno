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
 * File:   FTInitReplicaPacket.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2010, 3:00 PM
 */

#include "FTInitReplicaPacket.h"

FTInitReplicaPacket::FTInitReplicaPacket() :
m_params(0) {

}

FTInitReplicaPacket::FTInitReplicaPacket(InputStream& inputStream) :
m_params(0) {
    this->deserialize(inputStream);
}

FTInitReplicaPacket::FTInitReplicaPacket(
        ULong requestID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        UUIDPtr& rgid,
        UUIDPtr& sid,
        UUIDPtr& iid,
        ServiceParamsPtr& params,
        OverlayPeerInfoPtr& primary,
        list<OverlayPeerInfoPtr>& group
        ) :
SthenoStreamPacket(INIT_REPLICA_PACKET_OP, requestID),
m_uuid(uuid),
m_fid(fid),
m_rgid(rgid),
m_sid(sid),
        m_iid(iid),
m_params(params),
        m_primary(primary),
m_group(group) {
}

FTInitReplicaPacket::~FTInitReplicaPacket() {
}

void
FTInitReplicaPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& FTInitReplicaPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& FTInitReplicaPacket::getFID() {
    return m_fid;
}

UUIDPtr& FTInitReplicaPacket::getRGID() {
    return m_rgid;
}

UUIDPtr& FTInitReplicaPacket::getSID() {
    return m_sid;
}

UUIDPtr& FTInitReplicaPacket::getIID() {
    return m_iid;
}

ServiceParamsPtr& FTInitReplicaPacket::getServiceParams() {
    return m_params;
}

OverlayPeerInfoPtr& FTInitReplicaPacket::getPrimary(){
    return m_primary;
}


list<OverlayPeerInfoPtr>& FTInitReplicaPacket::getReplicationGroupMembers() {
    return m_group;
}

void
FTInitReplicaPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {    
    m_uuid->serialize(outputStream);
    //printf(":L:L:L:L:L:L:LFTInitReplicaPacket::serializeBody UUID(%s) RGID(%s)\n",m_uuid->toString().c_str(),m_rgid->toString().c_str());
    m_fid->serialize(outputStream);
    m_rgid->serialize(outputStream);
    m_sid->serialize(outputStream);
    m_iid->serialize(outputStream);
    m_params->serialize(outputStream);
    m_primary->serialize(outputStream);
    UInt size = m_group.size();
    outputStream.write_ulong(size);
    //printf("FTInitReplicaPacket::serializeBody group size %d\n",size);
    list<OverlayPeerInfoPtr>::iterator iter = m_group.begin();
    while (iter != m_group.end()) {
        //printf("FTInitReplicaPacket::serializeBody SAP %s\n",(*iter)->getUUID()->toString().c_str());
        (*iter)->serialize(outputStream);
        iter++;
    }
}

void
FTInitReplicaPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    UUID* rgid = new UUID(inputStream);
    m_rgid.reset(rgid);
    //printf(":D:D:D:D:D:D: FTInitReplicaPacket::deserializeBody UUID(%s) RGID(%s)\n",m_uuid->toString().c_str(),m_rgid->toString().c_str());
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* iid = new UUID(inputStream);
    m_iid.reset(iid);
    
    ServiceParams* params = new ServiceParams(inputStream);
    m_params.reset(params);
    OverlayPeerInfo* primary = new OverlayPeerInfo(inputStream);
    m_primary.reset(primary);
    UInt size = 0;
    inputStream.read_ulong(size);
    m_group.clear();
    for (int i = 0; i < size; i++) {
        OverlayPeerInfo* pi = new OverlayPeerInfo(inputStream);
        OverlayPeerInfoPtr piPtr(pi);
        m_group.push_back(piPtr);
    }

}