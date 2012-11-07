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
 * File:   GetSensorsQuery.cpp
 * Author: rmartins
 * 
 * Created on October 14, 2010, 12:28 PM
 */

#include "GetSensorsQuery.h"
#include "GeneralProvider.h"

GetSensorsQuery::GetSensorsQuery() :
DiscoveryQuery(GENERALPROVIDER_TYPE, GETSENSORS_QUERY) {

}

GetSensorsQuery::GetSensorsQuery(const GetSensorsQuery& orig) : DiscoveryQuery(orig.m_provider, orig.m_type) {
}

GetSensorsQuery::~GetSensorsQuery() {
}

GetSensorsQuery::GetSensorsQuery(DiscoveryQuery* query) : DiscoveryQuery(query) {
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}

void GetSensorsQuery::deserialize(InputStream& inputStream) throw (SerializationException&) {
    deserializeMetadata(inputStream);
    UInt size = 0;
    inputStream.read_ulong(size);
    deserializeQuerySpecialization(inputStream);
}

void GetSensorsQuery::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
}

void GetSensorsQuery::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void GetSensorsQuery::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
}
