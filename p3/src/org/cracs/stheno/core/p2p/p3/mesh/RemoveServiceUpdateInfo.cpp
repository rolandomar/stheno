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
 * File:   RemoveServiceUpdateInfo.cpp
 * Author: rmartins
 * 
 * Created on January 11, 2011, 1:08 PM
 */

#include "RemoveServiceUpdateInfo.h"

//Remove updateuinfo

RemoveServiceUpdateInfo::RemoveServiceUpdateInfo(UUIDPtr& pid, UUIDPtr& fid, UUIDPtr& sid, UUIDPtr& iid) :
MeshUpdateInfo(REMOVE_SERVICE_UPDATEINFO), m_pid(pid), m_fid(fid), m_sid(sid), m_iid(iid) {

}

RemoveServiceUpdateInfo::RemoveServiceUpdateInfo(InfoUpdate* info, InputStream& inputStream) :
MeshUpdateInfo(info->getOperation()) {
    delete info;
    this->deserializeSpecializationBody(inputStream);
}

RemoveServiceUpdateInfo::~RemoveServiceUpdateInfo() {
}

UUIDPtr& RemoveServiceUpdateInfo::getPID() {
    return m_pid;
}

UUIDPtr& RemoveServiceUpdateInfo::getFID() {
    return m_fid;
}

UUIDPtr& RemoveServiceUpdateInfo::getSID() {
    return m_sid;
}

UUIDPtr& RemoveServiceUpdateInfo::getIID() {
    return m_iid;

}

void RemoveServiceUpdateInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    MeshUpdateInfo::serialize(outputStream);
    serializeSpecializationBody(outputStream);
}

void RemoveServiceUpdateInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    MeshUpdateInfo::deserialize(inputStream);
    deserializeSpecializationBody(inputStream);
}

void RemoveServiceUpdateInfo::serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&) {
    m_pid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_sid->serialize(outputStream);
    m_iid->serialize(outputStream);
}

void RemoveServiceUpdateInfo::deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_pid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* iid = new UUID(inputStream);
    m_iid.reset(iid);
}


