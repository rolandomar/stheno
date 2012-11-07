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
 * File:   ChangegIIDServiceUpdateInfo.cpp
 * Author: rmartins
 * 
 * Created on January 11, 2011, 1:10 PM
 */

#include "ChangegIIDServiceUpdateInfo.h"


ChangegIIDServiceUpdateInfo::ChangegIIDServiceUpdateInfo(UUIDPtr& pid, UUIDPtr& fid, UUIDPtr& sid, UUIDPtr& oldIid, UUIDPtr& newIid) :
MeshUpdateInfo(UPDATE_IID_SERVICE_UPDATEINFO), m_pid(pid), m_fid(fid), m_sid(sid), m_oldIid(oldIid), m_newIid(newIid) {

}

ChangegIIDServiceUpdateInfo::ChangegIIDServiceUpdateInfo(InfoUpdate* info, InputStream& inputStream) :
MeshUpdateInfo(info->getOperation()) {
    delete info;
    this->deserializeSpecializationBody(inputStream);
}

ChangegIIDServiceUpdateInfo::~ChangegIIDServiceUpdateInfo() {
}

UUIDPtr& ChangegIIDServiceUpdateInfo::getPID() {
    return m_pid;
}

UUIDPtr& ChangegIIDServiceUpdateInfo::getFID() {
    return m_fid;
}

UUIDPtr& ChangegIIDServiceUpdateInfo::getSID() {
    return m_sid;
}

UUIDPtr& ChangegIIDServiceUpdateInfo::getOldIID() {
    return m_oldIid;
}

UUIDPtr& ChangegIIDServiceUpdateInfo::getNewIID() {
    return m_newIid;

}

void ChangegIIDServiceUpdateInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    MeshUpdateInfo::serialize(outputStream);
    serializeSpecializationBody(outputStream);
}

void ChangegIIDServiceUpdateInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    MeshUpdateInfo::deserialize(inputStream);
    deserializeSpecializationBody(inputStream);
}

void ChangegIIDServiceUpdateInfo::serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&) {
    m_pid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_sid->serialize(outputStream);
    m_oldIid->serialize(outputStream);
    m_newIid->serialize(outputStream);
}

void ChangegIIDServiceUpdateInfo::deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_pid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* oldIid = new UUID(inputStream);
    m_oldIid.reset(oldIid);
    UUID* newIid = new UUID(inputStream);
    m_newIid.reset(newIid);
}

