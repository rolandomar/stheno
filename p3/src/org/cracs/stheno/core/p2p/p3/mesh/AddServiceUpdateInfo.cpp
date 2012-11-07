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
 * File:   AddServiceUpdateInfo.cpp
 * Author: rmartins
 * 
 * Created on January 11, 2011, 1:05 PM
 */

#include "AddServiceUpdateInfo.h"

AddServiceUpdateInfo::AddServiceUpdateInfo(UUIDPtr& pid, UUIDPtr& fid, ServiceInstanceInfoPtr& info) :
MeshUpdateInfo(ADD_SERVICE_UPDATEINFO), m_info(info), m_pid(pid), m_fid(fid) {
}

AddServiceUpdateInfo::AddServiceUpdateInfo(InfoUpdate* info, InputStream& inputStream) :
MeshUpdateInfo(info->getOperation()) {
    delete info;
    this->deserializeSpecializationBody(inputStream);
}

AddServiceUpdateInfo::~AddServiceUpdateInfo() {
}

UUIDPtr& AddServiceUpdateInfo::getPID() {
    return m_pid;
}

UUIDPtr& AddServiceUpdateInfo::getFID() {
    return m_fid;
}

ServiceInstanceInfoPtr& AddServiceUpdateInfo::getServiceInfo() {
    return m_info;
}

void AddServiceUpdateInfo::serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&) {
    m_pid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_info->serialize(outputStream);
}

void AddServiceUpdateInfo::deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    //printf("AddServiceUpdateInfo uuid=%s\n", uuid->toString().c_str());
    m_pid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    //printf("AddServiceUpdateInfo fid=%s\n", fid->toString().c_str());
    m_fid.reset(fid);
    ServiceInstanceInfo* info = new ServiceInstanceInfo(inputStream);
    //printf("AddServiceUpdateInfo info=%s\n", info->toString().c_str());
    m_info.reset(info);
}

void AddServiceUpdateInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    MeshUpdateInfo::serialize(outputStream);
    serializeSpecializationBody(outputStream);
}

void AddServiceUpdateInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    MeshUpdateInfo::deserialize(inputStream);
    deserializeSpecializationBody(inputStream);
}

