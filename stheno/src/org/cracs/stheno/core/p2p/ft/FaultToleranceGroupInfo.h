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
 * File:   FaultToleranceGroupInfo.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 2:37 PM
 */

#ifndef FAULTTOLERANCEGROUPINFO_H
#define	FAULTTOLERANCEGROUPINFO_H

#include <ace/Synch.h>
#include <ace/Refcounted_Auto_Ptr.h>

#include <euryale/common/uuid/UUID.h>
#include <euryale/serialization/Serializable.h>

#include <stheno/core/p2p/ft/FTParticipantInfo.h>

class FaultToleranceGroupInfo : public Serializable {
public:
    FaultToleranceGroupInfo();
    virtual ~FaultToleranceGroupInfo();

    UInt getGroupSize();
    bool getPrimary(FTParticipantInfoPtr& primaryInfo);
    bool getReplica(FTParticipantInfoPtr& replicaInfo, UInt replicaIndex);

    bool removePrimary();
    bool removeReplica(int replicaIndex);
    /**
     * Removes replica using its uuid.
     * @param uuid Replica's uuid
     * @return Returns replica index.
     */
    int removeReplica(UUIDPtr& uuid);

    bool addPrimary(FTParticipantInfoPtr& info);
    bool addReplica(FTParticipantInfoPtr& info);
    bool remove(UUIDPtr& uuid);

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    list<FTParticipantInfoPtr> m_groupList;

    bool isMemberinGroup(UUIDPtr& uuid);
};

typedef ACE_Strong_Bound_Ptr<FaultToleranceGroupInfo, ACE_SYNCH_MUTEX> FaultToleranceGroupInfoPtr;

#endif	/* FAULTTOLERANCEGROUPINFO_H */

