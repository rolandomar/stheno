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
 * File:   DiscoveryQuery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 11, 2010, 5:53 PM
 */

#include "DiscoveryQuery.h"

DiscoveryQuery::DiscoveryQuery(UInt provider, UInt type) :
m_provider(provider), m_type(type), m_querySpecializationBlob(0) {
}

DiscoveryQuery::DiscoveryQuery(const DiscoveryQuery& orig) : m_querySpecializationBlob(0) {
    m_provider = orig.m_provider;
    m_type = orig.m_type;
    m_querySpecializationBlob = orig.m_querySpecializationBlob->clone();
}

DiscoveryQuery::DiscoveryQuery(InputStream& inputStream) :
m_provider(0), m_type(0), m_querySpecializationBlob(0) {
    this->deserialize(inputStream);
}

DiscoveryQuery::DiscoveryQuery(DiscoveryQuery* query) :
m_provider(0), m_type(0), m_querySpecializationBlob(0) {
    m_provider = query->m_provider;
    m_type = query->m_type;
    this->m_querySpecializationBlob = query->deattachQuerySpecializationBlob();
    delete query;
}

DiscoveryQuery::~DiscoveryQuery() {
}

UInt DiscoveryQuery::getProvider() {
    return m_provider;
}

UInt DiscoveryQuery::getType() {
    return m_type;
}

void DiscoveryQuery::serialize(OutputStream& outputStream) throw (SerializationException&) {
    serializeMetadata(outputStream);
    if (m_querySpecializationBlob != 0) {
        UInt size = m_querySpecializationBlob->total_length();
        outputStream.write_ulong(size);
        outputStream.write_octet_array_mb(m_querySpecializationBlob);
    } else {
        UInt size = getSerializationSizeQuerySpecialization();
        outputStream.write_ulong(size);
        serializeQuerySpecialization(outputStream);
    }
}

void DiscoveryQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: DiscoveryQuery::deserialize() tyoe=%d size=%d\n"), m_type, size));
    if (m_querySpecializationBlob != 0) {
        delete m_querySpecializationBlob;
        m_querySpecializationBlob = 0;

    }
    if (size > 0) {
        m_querySpecializationBlob = new ACE_Message_Block((size_t) size);
        inputStream.read_octet_array((Octet*) m_querySpecializationBlob->base(), size);
        m_querySpecializationBlob->wr_ptr(size);
    }
}

ACE_Message_Block* DiscoveryQuery::getQuerySpecializationBlob() {
    return m_querySpecializationBlob;
}

ACE_Message_Block* DiscoveryQuery::deattachQuerySpecializationBlob() {
    ACE_Message_Block* temp = m_querySpecializationBlob;
    m_querySpecializationBlob = 0;
    return temp;
}

void DiscoveryQuery::getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob) {
    if (m_querySpecializationBlob != 0) {
        delete m_querySpecializationBlob;
    }
    m_querySpecializationBlob = querySpecializationBlob;
}

void DiscoveryQuery::serializeMetadata(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_provider);
    outputStream.write_ulong(m_type);
}

void DiscoveryQuery::deserializeMetadata(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_provider);
    inputStream.read_ulong(m_type);
}

UInt DiscoveryQuery::getSerializationSizeQuerySpecialization() throw (SerializationException&) {
    StreamSize streamSize;
    serializeQuerySpecialization(streamSize);
    return streamSize.total_length();
}

void DiscoveryQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
}
