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
 * File:   FTParticipantInfo.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 3:05 PM
 */

#include "FTParticipantInfo.h"

FTParticipantInfo::FTParticipantInfo(UUIDPtr& uuid, SAPInfoPtr& ftControlSAP, SAPInfoPtr& ftDataSAP) :
m_uuid(uuid), m_ftControlSAP(ftControlSAP), m_ftDataSAP(ftDataSAP) {
}

FTParticipantInfo::FTParticipantInfo(InputStream& inputStream) {
    this->deserialize(inputStream);
}

FTParticipantInfo::~FTParticipantInfo() {
}

UUIDPtr& FTParticipantInfo::getUUID() {
    return m_uuid;
}

SAPInfoPtr& FTParticipantInfo::getFTControlSAP() {
    return m_ftControlSAP;
}

SAPInfoPtr& FTParticipantInfo::getFTDataSAP() {
    return m_ftDataSAP;
}

void FTParticipantInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_ftControlSAP->serialize(outputStream);
    if (!m_ftDataSAP.null()) {
        outputStream.write_boolean(true);
        m_ftDataSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
}

void FTParticipantInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    SAPInfo* sapInfo = new SAPInfo(inputStream);
    m_ftControlSAP.reset(sapInfo);
    Boolean flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* dataSAPInfo = new SAPInfo(inputStream);
        m_ftDataSAP.reset(dataSAPInfo);
    } else {
        m_ftDataSAP.reset(0);
    }
}