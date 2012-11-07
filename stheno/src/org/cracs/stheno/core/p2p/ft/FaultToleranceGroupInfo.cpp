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
 * File:   FaultToleranceGroupInfo.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 2:37 PM
 */

#include "FaultToleranceGroupInfo.h"

FaultToleranceGroupInfo::FaultToleranceGroupInfo() {
}

FaultToleranceGroupInfo::~FaultToleranceGroupInfo() {
}

UInt FaultToleranceGroupInfo::getGroupSize() {
    return m_groupList.size();
}

bool FaultToleranceGroupInfo::getPrimary(FTParticipantInfoPtr& primaryInfo) {
    if (getGroupSize() == 0) {
        return false;
    }
    primaryInfo = m_groupList.front();
    return true;
}

bool FaultToleranceGroupInfo::getReplica(FTParticipantInfoPtr& replicaInfo, UInt replicaIndex) {
    if (replicaIndex > getGroupSize() - 1) {
        return false;
    }
    replicaIndex++; //bypass primary
    int i = 0;
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    while (iter != m_groupList.end()) {
        if (i == replicaIndex) {
            replicaInfo = (*iter);
            return true;
        }
        i++;
        iter++;
    }
    return false;
}

bool FaultToleranceGroupInfo::removePrimary() {
    if (getGroupSize() == 0) {
        return false;
    }
    m_groupList.pop_front();
    return true;
}

bool FaultToleranceGroupInfo::removeReplica(int replicaIndex) {
    if (replicaIndex > getGroupSize() - 1) {
        return false;
    }
    replicaIndex++; //bypass primary
    int i = 0;
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    while (iter != m_groupList.end()) {
        if (i == replicaIndex) {
            m_groupList.erase(iter);
            return true;
        }
        i++;
        iter++;
    }
    return false;
}

/**
 * Removes replica using its uuid.
 * @param uuid Replica's uuid
 * @return Returns replica index.
 */
int FaultToleranceGroupInfo::removeReplica(UUIDPtr& uuid) {
    if (getGroupSize() <= 1) {
        return -1;
    }
    int i = 0;
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    iter++; //bypass primary        
    while (iter != m_groupList.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            m_groupList.erase(iter);
            return i;
        }
        i++;
        iter++;
    }
    return -1;
}

bool FaultToleranceGroupInfo::addPrimary(FTParticipantInfoPtr& info) {
    if (m_groupList.size() != 0) {
        return false;
    }
    if (isMemberinGroup(info->getUUID())) {
        return false;
    }
    m_groupList.push_back(info);
    return true;
}

bool FaultToleranceGroupInfo::addReplica(FTParticipantInfoPtr& info) {
    if (m_groupList.size() == 0) {
        return false;
    }
    if (isMemberinGroup(info->getUUID())) {
        return false;
    }
    m_groupList.push_back(info);
    return true;
}

bool FaultToleranceGroupInfo::remove(UUIDPtr& uuid) {
    if (!isMemberinGroup(uuid)) {
        return false;
    }
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    while (iter != m_groupList.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            m_groupList.erase(iter);
            return true;
        }
        iter++;
    }

    return false;
}

void FaultToleranceGroupInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt size = m_groupList.size();
    outputStream.write_ulong(size);
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    while (iter != m_groupList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
}

void FaultToleranceGroupInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    m_groupList.clear();
    UInt size = 0;
    inputStream.read_ulong(size);
    for (int i = 0; i < size; i++) {
        FTParticipantInfo* info = new FTParticipantInfo(inputStream);
        FTParticipantInfoPtr infoPtr(info);
        m_groupList.push_back(infoPtr);
    }
}

bool FaultToleranceGroupInfo::isMemberinGroup(UUIDPtr& uuid) {
    list<FTParticipantInfoPtr>::iterator iter = m_groupList.begin();
    while (iter != m_groupList.end()) {
        if ((*iter)->getUUID()->isEqual(uuid)) {
            return true;
        }
        iter++;
    }
    return false;
}
