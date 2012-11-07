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
 * File:   PeerQuery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 13, 2010, 2:30 PM
 */

#include "PeerQuery.h"
#include "PeerProvider.h"

const UInt PeerQuery::ROUTING_QUERY = 0x32DCAA;

PeerQuery::PeerQuery(UUIDPtr& uuid) : DiscoveryQuery(PeerProvider::PEER_PROVIDER, ROUTING_QUERY),
m_uuid(uuid) {

}

PeerQuery::PeerQuery(const PeerQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type) {
    m_uuid = orig.m_uuid;
}

PeerQuery::~PeerQuery() {
}

PeerQuery::PeerQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

UUIDPtr& PeerQuery::getUUID() {
    return m_uuid;
}

void PeerQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void PeerQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
}

void PeerQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void PeerQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
}