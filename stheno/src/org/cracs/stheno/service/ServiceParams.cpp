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
 * File:   ServiceParams.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 25, 2010, 3:15 PM
 */

#include "ServiceParams.h"

ServiceParams::ServiceParams(const UUIDPtr& sid, FTServiceParamsPtr& ftParams, ACE_Message_Block* querySpecializationBlob) :
m_sid(sid), m_ftParams(ftParams), m_querySpecializationBlob(querySpecializationBlob) {
}

ServiceParams::ServiceParams(ServiceParams* params) :
m_querySpecializationBlob(0) {
    m_sid = params->m_sid;
    m_ftParams = params->m_ftParams;
    this->m_querySpecializationBlob = params->deattachQuerySpecializationBlob();
    delete params;
}

ServiceParams::ServiceParams(const UUIDPtr& sid, FTServiceParamsPtr& ftParams) :
m_sid(sid), m_ftParams(ftParams), m_querySpecializationBlob(0) {
}

ServiceParams::ServiceParams(InputStream& inputStream) :
m_querySpecializationBlob(0) {
    this->deserialize(inputStream);
}

ServiceParams::ServiceParams(const UUIDPtr& sid) :
m_sid(sid), m_querySpecializationBlob(0) {
}

ServiceParams* ServiceParams::duplicate() {
    ACE_Message_Block* specilization = 0;
    if (m_querySpecializationBlob != 0) {
        specilization = m_querySpecializationBlob->duplicate();
    }
    ServiceParams* dup = new ServiceParams(m_sid, m_ftParams, specilization);
    return dup;
}


ServiceParams::~ServiceParams() {
}

FTServiceParamsPtr& ServiceParams::getFTParams() {
    return m_ftParams;
}

UUIDPtr& ServiceParams::getSID() {
    return m_sid;
}

void ServiceParams::serialize(OutputStream& outputStream) throw (SerializationException&) {
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

void ServiceParams::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
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

ACE_Message_Block* ServiceParams::getQuerySpecializationBlob() {
    return m_querySpecializationBlob;
}

ACE_Message_Block* ServiceParams::deattachQuerySpecializationBlob() {
    ACE_Message_Block* temp = m_querySpecializationBlob;
    m_querySpecializationBlob = 0;
    return temp;
}

void ServiceParams::getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob) {
    if (m_querySpecializationBlob != 0) {
        delete m_querySpecializationBlob;
    }
    m_querySpecializationBlob = querySpecializationBlob;
}

void ServiceParams::serializeMetadata(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    if (!m_ftParams.null()) {
        outputStream.write_boolean(true);
        m_ftParams->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
}

void ServiceParams::deserializeMetadata(InputStream& inputStream) throw (SerializationException&) {

    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    bool flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        FTServiceParams* ftParams = new FTServiceParams(inputStream);
        m_ftParams.reset(ftParams);
    } else {
        m_ftParams.reset(0);
    }
}

UInt ServiceParams::getSerializationSizeQuerySpecialization() throw (SerializationException&) {
    StreamSize streamSize;
    serializeQuerySpecialization(streamSize);
    return streamSize.total_length();
}

void ServiceParams::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    /*if (m_querySpecializationBlob != 0) {
        UInt size = m_querySpecializationBlob->total_length();
        outputStream.write_ulong(size);
        outputStream.write_octet_array_mb(m_querySpecializationBlob);
    } else {
        UInt size = getSerializationSizeQuerySpecialization();           
        outputStream.write_ulong(size);
        serializeQuerySpecialization(outputStream);
    }*/
}