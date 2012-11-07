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
 * File:   P3DiscoveryQueryPacket.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 4:16 PM
 */

#include "P3DiscoveryQueryPacket.h"
#include "stheno/core/p2p/discovery/DiscoveryQuery.h"

P3DiscoveryQueryPacket::P3DiscoveryQueryPacket() {
}

P3DiscoveryQueryPacket::P3DiscoveryQueryPacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

P3DiscoveryQueryPacket::P3DiscoveryQueryPacket(
        ULong requestID, 
        UUIDPtr& uuid, 
        UUIDPtr& fid, 
        QoSEndpoint& qos,
        DiscoveryQueryPtr& query) :
SthenoStreamPacket(P3DISCOVERY_QUERY_PACKET, requestID), m_uuid(uuid),
m_fid(fid), m_qos(qos), m_query(query) {
}

P3DiscoveryQueryPacket::P3DiscoveryQueryPacket(
        ULong requestID, 
        UUIDPtr& uuid, 
        UUIDPtr& fid, 
        QoSEndpoint& qos,
        DiscoveryQuery* query) :
SthenoStreamPacket(P3DISCOVERY_QUERY_PACKET, requestID), m_uuid(uuid),
m_fid(fid), m_qos(qos), m_query(query) {
}

P3DiscoveryQueryPacket::~P3DiscoveryQueryPacket() {
}

void
P3DiscoveryQueryPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& P3DiscoveryQueryPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& P3DiscoveryQueryPacket::getFID() {
    return m_fid;
}

QoSEndpoint& P3DiscoveryQueryPacket::getQoS() {
    return m_qos;
}

DiscoveryQueryPtr& P3DiscoveryQueryPacket::getQuery(){
    return m_query;
}

void
P3DiscoveryQueryPacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_qos.serialize(outputStream);
    m_query->serialize(outputStream);
}

void
P3DiscoveryQueryPacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    m_qos.deserialize(inputStream);
    DiscoveryQuery* query = new DiscoveryQuery(inputStream);
    m_query.reset(query);

}


