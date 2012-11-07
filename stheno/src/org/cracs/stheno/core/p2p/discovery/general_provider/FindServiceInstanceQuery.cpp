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
 * File:   FindServiceInstanceQuery.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 7:22 PM
 */

#include "FindServiceInstanceQuery.h"

#include "GeneralProvider.h"

FindServiceInstanceQuery::FindServiceInstanceQuery(const UUIDPtr& sid, const UUIDPtr& iid) :
DiscoveryQuery(GENERALPROVIDER_TYPE, FINDSERVICEINSTANCE_QUERY), m_sid(sid), m_iid(iid) {

}

FindServiceInstanceQuery::FindServiceInstanceQuery(const FindServiceInstanceQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type),
m_sid(orig.m_sid), m_iid(orig.m_iid) {
}

FindServiceInstanceQuery::~FindServiceInstanceQuery() {
}

FindServiceInstanceQuery::FindServiceInstanceQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

void FindServiceInstanceQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void FindServiceInstanceQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    m_iid->serialize(outputStream);
}

UUIDPtr& FindServiceInstanceQuery::getSID() {
    return m_sid;
}

UUIDPtr& FindServiceInstanceQuery::getIID() {
    return m_iid;
}

void FindServiceInstanceQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void FindServiceInstanceQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* iid = new UUID(inputStream);
    m_iid.reset(iid);
}