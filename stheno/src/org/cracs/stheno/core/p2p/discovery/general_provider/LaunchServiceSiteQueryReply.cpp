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
 * File:   LaunchServiceSiteQueryReply.cpp
 * Author: rmartins
 * 
 * Created on November 15, 2010, 5:45 PM
 */

#include "LaunchServiceSiteQueryReply.h"
#include "GeneralProvider.h"

LaunchServiceSiteQueryReply::LaunchServiceSiteQueryReply(UUIDPtr& uuid, UUIDPtr& fid, SAPInfoPtr& meshSAP, SAPInfoPtr& ftSAP) :
DiscoveryQueryReply(GENERALPROVIDER_TYPE, LAUNCHSERVICESITE_QUERY_REPLY),
m_uuid(uuid), m_fid(fid), m_meshSAP(meshSAP), m_ftSAP(ftSAP) {

}

LaunchServiceSiteQueryReply::~LaunchServiceSiteQueryReply() {

}

LaunchServiceSiteQueryReply::LaunchServiceSiteQueryReply(DiscoveryQueryReply* query) : DiscoveryQueryReply(query) {
    if (m_ex == 0) {
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

void LaunchServiceSiteQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
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

void LaunchServiceSiteQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_fid->serialize(outputStream);
    m_meshSAP->serialize(outputStream);
    m_ftSAP->serialize(outputStream);
}

UUIDPtr& LaunchServiceSiteQueryReply::getUUID() {
    return m_uuid;
}

UUIDPtr& LaunchServiceSiteQueryReply::getFID() {
    return m_fid;
}

SAPInfoPtr& LaunchServiceSiteQueryReply::getMeshSAPInfo() {
    return m_meshSAP;
}

SAPInfoPtr& LaunchServiceSiteQueryReply::getFTSAPInfo() {
    return m_ftSAP;
}

void LaunchServiceSiteQueryReply::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void LaunchServiceSiteQueryReply::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    SAPInfo* sapInfo = new SAPInfo(inputStream);
    m_meshSAP.reset(sapInfo);
    SAPInfo* ftSAPInfo = new SAPInfo(inputStream);
    m_ftSAP.reset(ftSAPInfo);
}

