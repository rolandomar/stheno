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
 * File:   DiscoveryQueryReply.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 12, 2010, 10:57 AM
 */

#include "DiscoveryQueryReply.h"

DiscoveryQueryReply::DiscoveryQueryReply(UInt provider, UInt type) :
m_provider(provider), m_type(type), m_ex(0), m_querySpecializationBlob(0) {
}

DiscoveryQueryReply::DiscoveryQueryReply(InputStream& inputStream) :
m_ex(0), m_querySpecializationBlob(0) {
    this->deserialize(inputStream);
}

DiscoveryQueryReply::DiscoveryQueryReply(UInt provider, UInt type, DiscoveryException* ex) :
m_provider(provider), m_type(type), m_ex(ex), m_querySpecializationBlob(0) {
}

/*DiscoveryQueryReply::DiscoveryQueryReply(DiscoveryException* ex) :
m_type(EXCEPTION_REPLY), m_ex(ex) {
}*/

DiscoveryQueryReply::DiscoveryQueryReply(const DiscoveryQueryReply& orig) :
m_ex(0), m_querySpecializationBlob(0) {
    m_type = orig.m_type;
    m_ex = new DiscoveryException(*orig.m_ex);
}

DiscoveryQueryReply::DiscoveryQueryReply(DiscoveryQueryReply* query) :
m_ex(0), m_querySpecializationBlob(0) {
    m_type = query->m_type;
    DiscoveryException* ex = query->deattachException();
    if (ex == 0) {
        this->m_querySpecializationBlob = query->deattachQuerySpecializationBlob();
    }
    delete query;
}

DiscoveryQueryReply::~DiscoveryQueryReply() {
    //printf("DiscoveryQueryReply::~DiscoveryQueryReply()\n");
    if (m_ex != 0) {
        delete m_ex;        
    }
    ACE_Message_Block::release(m_querySpecializationBlob);
}

UInt DiscoveryQueryReply::getType() {
    return m_type;
}

UInt DiscoveryQueryReply::getProvider() {
    return m_provider;
}

void DiscoveryQueryReply::serialize(OutputStream& outputStream) throw (SerializationException&) {
    serializeMetadata(outputStream);
    //exception?
    //printf("DiscoveryReply:serialize 0\n");
    if (m_ex != 0) {
        //printf("DiscoveryReply:serialize 1\n");
        outputStream.write_boolean(true);
        m_ex->serialize(outputStream);
    } else {
        //printf("DiscoveryReply:serialize 2\n");
        outputStream.write_boolean(false);
        //printf("DiscoveryReply:serialize 3\n");
        UInt size = getSerializationSizeQuerySpecialization();
        //printf("DiscoveryReply:serialize 4\n");
        outputStream.write_ulong(size);
        //printf("DiscoveryReply:serialize 5 SIZE=%d\n",size);
        serializeQuerySpecialization(outputStream);
        //printf("DiscoveryReply:serialize 6\n");
    }

}

void DiscoveryQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
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
        //printf("****************************************** SIZE=%d\n",size);
        if (m_querySpecializationBlob != 0) {
            //printf("****************************************** werewjrewrewSIZE=%d\n",size);
            delete m_querySpecializationBlob;
            m_querySpecializationBlob = 0;

        }
        if (size > 0) {
            //printf("****************************************** dsnnmfdnmsdnmfnmdfs SIZE=%d\n",size);
            m_querySpecializationBlob = new ACE_Message_Block((size_t) size);
            inputStream.read_octet_array((Octet*) m_querySpecializationBlob->base(), size);
            m_querySpecializationBlob->wr_ptr(size);
        }
    }
}

ACE_Message_Block* DiscoveryQueryReply::getQuerySpecializationBlob() {
    return m_querySpecializationBlob;
}

ACE_Message_Block* DiscoveryQueryReply::deattachQuerySpecializationBlob() {
    ACE_Message_Block* temp = m_querySpecializationBlob;
    m_querySpecializationBlob = 0;
    return temp;
}

void DiscoveryQueryReply::getQuerySpecializationBlob(ACE_Message_Block* querySpecializationBlob) {
    if (m_querySpecializationBlob != 0) {
        delete m_querySpecializationBlob;
    }
    m_querySpecializationBlob = querySpecializationBlob;
}

DiscoveryException* DiscoveryQueryReply::deattachException() {
    DiscoveryException* temp = m_ex;
    m_ex = 0;
    return temp;
}

bool DiscoveryQueryReply::isException() {
    return (m_ex != 0);
}

void DiscoveryQueryReply::serializeMetadata(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_provider);
    outputStream.write_ulong(m_type);
}

void DiscoveryQueryReply::deserializeMetadata(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_provider);
    inputStream.read_ulong(m_type);
}

UInt DiscoveryQueryReply::getSerializationSizeQuerySpecialization() throw (SerializationException&) {
    StreamSize streamSize;
    serializeQuerySpecialization(streamSize);
    return streamSize.total_length();
}

void DiscoveryQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    if (m_querySpecializationBlob != 0) {
        outputStream.write_octet_array_mb(m_querySpecializationBlob);
    }
}
