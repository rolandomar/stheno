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
 * File:   P3FindPeerQuery.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 7:09 PM
 */

#include "P3FindPeerQuery.h"
#include <stheno/core/p2p/discovery/general_provider/GeneralProvider.h>

P3FindPeerQuery::P3FindPeerQuery(int treeLevel) :
DiscoveryQuery(GENERALPROVIDER_TYPE, P3FINDPEER_QUERY), m_treeLevel(treeLevel) {

}

P3FindPeerQuery::P3FindPeerQuery(const P3FindPeerQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type),
m_treeLevel(orig.m_treeLevel) {
}

P3FindPeerQuery::~P3FindPeerQuery() {
}

P3FindPeerQuery::P3FindPeerQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

void P3FindPeerQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void P3FindPeerQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_long(m_treeLevel);    
}

int P3FindPeerQuery::getTreeLevel() {
    return m_treeLevel;
}

void P3FindPeerQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void P3FindPeerQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_long(m_treeLevel);
}
