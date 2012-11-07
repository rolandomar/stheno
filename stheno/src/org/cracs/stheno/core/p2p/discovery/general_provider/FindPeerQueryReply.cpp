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
 * File:   FindQueryReply.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 7:09 PM
 */

#include "FindPeerQueryReply.h"

#include "GeneralProvider.h"

FindPeerQueryReply::FindPeerQueryReply(PeerInfoPtr& pi) :
DiscoveryQueryReply(GENERALPROVIDER_TYPE, FINDPEER_QUERY_REPLY),
m_pi(pi) {
}

FindPeerQueryReply::~FindPeerQueryReply() {
}

FindPeerQueryReply::FindPeerQueryReply(DiscoveryQueryReply* query) : DiscoveryQueryReply(query) {
    if (m_ex == 0) {
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

void FindPeerQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
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

void FindPeerQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_pi->serialize(outputStream);
}

PeerInfoPtr& FindPeerQueryReply::getPeerInfo() {
    return m_pi;
}

void FindPeerQueryReply::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void FindPeerQueryReply::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    PeerInfo* pi = new PeerInfo(inputStream);
    printf("OOOOOOOOOOOOOOOOOOOOOOO=%s %s\n",pi->getPID()->toString().c_str(),pi->getFID()->toString().c_str());
    m_pi.reset(pi);
}

