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
 * File:   FindPeerQuery.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 7:09 PM
 */

#include "FindPeerQuery.h"
#include "GeneralProvider.h"

FindPeerQuery::FindPeerQuery(UUIDPtr& uuid) :
DiscoveryQuery(GENERALPROVIDER_TYPE, FINDPEER_QUERY), m_uuid(uuid) {

}

FindPeerQuery::FindPeerQuery(const FindPeerQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type),
m_uuid(orig.m_uuid) {
}

FindPeerQuery::~FindPeerQuery() {
}

FindPeerQuery::FindPeerQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

void FindPeerQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void FindPeerQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
}

UUIDPtr& FindPeerQuery::getUUID() {
    return m_uuid;
}

void FindPeerQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void FindPeerQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* iid = new UUID(inputStream);
    m_uuid.reset(iid);
}
