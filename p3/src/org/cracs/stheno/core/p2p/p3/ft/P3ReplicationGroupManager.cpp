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

#include "P3ReplicationGroupManager.h"

P3ReplicationGroupManager::P3ReplicationGroupManager() {
}

P3ReplicationGroupManager::~P3ReplicationGroupManager() {
}

bool P3ReplicationGroupManager::addReplicationGroup(ReplicationGroupPtr& rg) {
    printf("\n\n RGID=%s\n\n",rg->getGroupUUID()->toString().c_str());
    if (m_replicationGroupMap.bind(rg->getGroupUUID(), rg) == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ReplicationGroupManager::addReplicationGroup ok\n")));
        return true;
    }
    printf("\n\n RGID=%s FAILED\n\n",rg->getGroupUUID()->toString().c_str());
    return false;
}

bool P3ReplicationGroupManager::removeReplicationGroup(const UUIDPtr& rgid) {
    if (m_replicationGroupMap.unbind(rgid) == 0) {
        return true;
    }
    return false;
}

bool P3ReplicationGroupManager::getReplicationGroup(const UUIDPtr& rgid, ReplicationGroupPtr& rg){
    if (m_replicationGroupMap.find(rgid, rg) == 0) {
        return true;
    }
    return false;
}