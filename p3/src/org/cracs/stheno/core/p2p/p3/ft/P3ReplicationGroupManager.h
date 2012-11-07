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
 * File:   ReplicationGroupManager.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 6:49 PM
 */

#ifndef REPLICATIONGROUPMANAGER_H
#define	REPLICATIONGROUPMANAGER_H

//#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>
#include <stheno/core/p2p/ft/ReplicationGroup.h>

class P3ReplicationGroupManager {
public:
    P3ReplicationGroupManager();
    virtual ~P3ReplicationGroupManager();

    bool addReplicationGroup(ReplicationGroupPtr& rg);
    bool removeReplicationGroup(const UUIDPtr& rgid);
    bool getReplicationGroup(const UUIDPtr& rgid, ReplicationGroupPtr& rg);

protected:
    ACE_Hash_Map_Manager<UUIDPtr, ReplicationGroupPtr, ACE_SYNCH_RW_MUTEX> m_replicationGroupMap;
};

#endif	/* REPLICATIONGROUPMANAGER_H */

