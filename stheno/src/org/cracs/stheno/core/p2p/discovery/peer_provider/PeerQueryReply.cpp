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
 * File:   PeerQueryReply.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 13, 2010, 2:40 PM
 */

#include "PeerQueryReply.h"
#include "PeerProvider.h"

PeerQueryReply::PeerQueryReply(PeerInfoPtr& info) : DiscoveryQueryReply(PeerProvider::PEER_PROVIDER, ROUTING_QUERY_REPLY), m_info(info) {

}

PeerQueryReply::~PeerQueryReply() {

}

PeerQueryReply::PeerQueryReply(DiscoveryQueryReply* query) : DiscoveryQueryReply(query) {
    if (m_ex == 0) {
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

PeerInfoPtr& PeerQueryReply::getPeerInfo() {
    return m_info;
}

void PeerQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
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

void PeerQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_info->serialize(outputStream);
}

void PeerQueryReply::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void PeerQueryReply::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    PeerInfo* peerInfo = new PeerInfo(inputStream);
    m_info.reset(peerInfo);
}