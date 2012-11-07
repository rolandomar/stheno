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
 * File:   OverlayPeerInfo.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 5:42 PM
 */

#include "OverlayPeerInfo.h"

OverlayPeerInfo::OverlayPeerInfo(
        UUIDPtr& uuid,
        UUIDPtr& fid,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP,
        SAPInfoPtr& ftDataSAP) :
m_uuid(uuid), m_fid(fid), m_meshSAP(meshSAP), m_ftSAP(ftSAP), m_ftDataSAP(ftDataSAP) {
}

OverlayPeerInfo::OverlayPeerInfo(
        UUIDPtr& uuid,
        UUIDPtr& fid,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP) :
m_uuid(uuid), m_fid(fid), m_meshSAP(meshSAP), m_ftSAP(ftSAP) {
}

OverlayPeerInfo::OverlayPeerInfo(InputStream& inputStream) {
    this->deserialize(inputStream);
}

OverlayPeerInfo::OverlayPeerInfo(const OverlayPeerInfo& orig) :
m_uuid(orig.m_uuid), m_fid(orig.m_fid), m_meshSAP(orig.m_meshSAP), m_ftSAP(orig.m_ftSAP) {
}

OverlayPeerInfo::~OverlayPeerInfo() {
}

UUIDPtr& OverlayPeerInfo::getUUID() {
    return m_uuid;
}

UUIDPtr& OverlayPeerInfo::getFID() {
    return m_fid;
}

SAPInfoPtr& OverlayPeerInfo::getMeshSAPInfo() {
    return m_meshSAP;
}

SAPInfoPtr& OverlayPeerInfo::getFTSAPInfo() {
    return m_ftSAP;
}

bool OverlayPeerInfo::hasFTDataSAP() {
    return !m_ftDataSAP.null();
}

void OverlayPeerInfo::setFTDataSAP(SAPInfoPtr& ftDataSAP) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: OverlayPeerInfo::setFTDataSAP() - info(%@)\n"), ftDataSAP.get()));
    m_ftDataSAP = ftDataSAP;
}

SAPInfoPtr& OverlayPeerInfo::getFTDataSAPInfo() {
    return m_ftDataSAP;
}

void OverlayPeerInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_meshSAP->serialize(outputStream);
    m_ftSAP->serialize(outputStream);
    m_ftDataSAP->serialize(outputStream);
}

void OverlayPeerInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    SAPInfo* meshSAP = new SAPInfo(inputStream);
    m_meshSAP.reset(meshSAP);
    SAPInfo* ftSAP = new SAPInfo(inputStream);
    m_ftSAP.reset(ftSAP);
    SAPInfo* ftDataSAP = new SAPInfo(inputStream);
    m_ftDataSAP.reset(ftDataSAP);
}
