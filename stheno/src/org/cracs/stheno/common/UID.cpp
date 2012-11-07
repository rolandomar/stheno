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
 * File:   UID.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 13, 2010, 4:42 PM
 */

#include "UID.h"

UID::UID(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& sid) :
m_uuid(uuid), m_fid(fid),m_sid(sid) {    
}

UID::UID(const UID& orig) : m_uuid(orig.m_uuid), m_fid(orig.m_fid),m_sid(orig.m_sid) {    
}

UID::~UID() {
}

UUIDPtr& UID::getPID() {
    return m_uuid;
}

UUIDPtr& UID::getFID() {
    return m_fid;
}

UUIDPtr& UID::getSID() {
    return m_sid;
}

void UID::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_sid->serialize(outputStream);
}

void UID::deserialize(InputStream& inputStream) throw (SerializationException&) {    
    m_uuid.reset(new UUID(inputStream));
    m_fid.reset(new UUID(inputStream));
    m_sid.reset(new UUID(inputStream));
}
