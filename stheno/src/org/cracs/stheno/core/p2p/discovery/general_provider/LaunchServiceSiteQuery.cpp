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
 * File:   LaunchServiceSiteQuery.cpp
 * Author: rmartins
 * 
 * Created on November 15, 2010, 5:45 PM
 */

#include "LaunchServiceSiteQuery.h"
#include "GeneralProvider.h"

LaunchServiceSiteQuery::LaunchServiceSiteQuery(const UUIDPtr& requester, const UUIDPtr& sid, list<UUIDPtr>& filterPeers) :
DiscoveryQuery(GENERALPROVIDER_TYPE, LAUNCHSERVICESITE_QUERY),
m_requester(requester), m_sid(sid), m_filterPeers(filterPeers) {

}

LaunchServiceSiteQuery::LaunchServiceSiteQuery(const LaunchServiceSiteQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type),
m_requester(orig.m_requester), m_sid(orig.m_sid), m_filterPeers(orig.m_filterPeers) {
}

LaunchServiceSiteQuery::~LaunchServiceSiteQuery() {
}

LaunchServiceSiteQuery::LaunchServiceSiteQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

void LaunchServiceSiteQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void LaunchServiceSiteQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    m_requester->serialize(outputStream);
    UInt size = m_filterPeers.size();
    outputStream.write_long(size);
    list<UUIDPtr>::iterator iter = m_filterPeers.begin();
    while (iter != m_filterPeers.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }

    //printf("\n\nOOOOOOOOOOOOOOOOOOOOOOOOOOOO%s %s\n",m_requester->toString().c_str(),m_sid->toString().c_str());
}

UUIDPtr& LaunchServiceSiteQuery::getSID() {
    return m_sid;
}

UUIDPtr& LaunchServiceSiteQuery::getRequester() {
    return m_requester;
}

list<UUIDPtr>& LaunchServiceSiteQuery::getFilterPeers() {
    return m_filterPeers;
}

void LaunchServiceSiteQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void LaunchServiceSiteQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* requester = new UUID(inputStream);
    m_requester.reset(requester);

    UInt size = 0;
    m_filterPeers.clear();
    inputStream.read_ulong(size);
    for (int i = 0; i < size; i++) {
        UUID* uuid = new UUID(inputStream);
        UUIDPtr uuidPtr(uuid);
        m_filterPeers.push_back(uuidPtr);

    }
}
