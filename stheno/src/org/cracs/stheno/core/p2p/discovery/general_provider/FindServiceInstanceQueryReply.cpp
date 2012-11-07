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
 * File:   FindServiceInstanceQueryReply.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 7:22 PM
 */

#include "FindServiceInstanceQueryReply.h"

#include "GeneralProvider.h"

FindServiceInstanceQueryReply::FindServiceInstanceQueryReply(UUIDPtr& sid, UUIDPtr& fid, SAPInfoPtr& sapInfo) :
DiscoveryQueryReply(GENERALPROVIDER_TYPE, FINDSERVICEINSTANCE_QUERY_REPLY),
m_sid(sid), m_fid(fid), m_sapInfo(sapInfo) {
}

FindServiceInstanceQueryReply::~FindServiceInstanceQueryReply() {
}

FindServiceInstanceQueryReply::FindServiceInstanceQueryReply(DiscoveryQueryReply* query) : DiscoveryQueryReply(query) {
    if (m_ex == 0) {
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

void FindServiceInstanceQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    //delete m_ex if present
    if (m_ex != 0) {
        delete m_ex;
        m_ex = 0;
    }
    //end clear
    Boolean flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        m_ex = new DiscoveryException(inputStream);
    } else {
        UInt size = 0;
        inputStream.read_ulong(size);
        deserializeQuerySpecialization(inputStream);
    }
}

void FindServiceInstanceQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_sapInfo->serialize(outputStream);
}

UUIDPtr& FindServiceInstanceQueryReply::getSID() {
    return m_sid;
}

UUIDPtr& FindServiceInstanceQueryReply::getFID() {
    return m_fid;
}

SAPInfoPtr& FindServiceInstanceQueryReply::getSAPInfo() {
    return m_sapInfo;
}

void FindServiceInstanceQueryReply::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void FindServiceInstanceQueryReply::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* fid = new UUID(inputStream);
    m_sid.reset(fid);
    SAPInfo* sapInfo = new SAPInfo(inputStream);
    m_sapInfo.reset(sapInfo);
}