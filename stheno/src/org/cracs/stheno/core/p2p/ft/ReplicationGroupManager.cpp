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
 * File:   ReplicationGroupManager.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 6:49 PM
 */

#include "ReplicationGroupManager.h"

ReplicationGroupManager::ReplicationGroupManager() {
}

ReplicationGroupManager::~ReplicationGroupManager() {
}

bool ReplicationGroupManager::addReplicationGroup(ReplicationGroupPtr& rg) {
    if (m_replicationGroupMap.bind(rg->getGroupUUID(), rg) == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ReplicationGroupManager::addReplicationGroup ok\n")));
        return true;
    }
    return false;
}

bool ReplicationGroupManager::removeReplicationGroup(const UUIDPtr& rgid) {
    if (m_replicationGroupMap.unbind(rgid) == 0) {
        return true;
    }
    return false;
}

bool ReplicationGroupManager::getReplicationGroup(const UUIDPtr& rgid, ReplicationGroupPtr& rg){
    if (m_replicationGroupMap.find(rgid, rg) == 0) {
        return true;
    }
    return false;
}