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
 * File:   FTParticipantInfo.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 3:05 PM
 */

#ifndef FTPARTICIPANTINFO_H
#define	FTPARTICIPANTINFO_H


#include <ace/Synch.h>
#include <ace/Refcounted_Auto_Ptr.h>

#include <euryale/common/uuid/UUID.h>
#include <euryale/serialization/Serializable.h>
#include <euryale/net/endpoint/SAPInfo.h>

class FTParticipantInfo : public Serializable {
public:
    FTParticipantInfo(UUIDPtr& uuid, SAPInfoPtr& ftControlSAP, SAPInfoPtr& ftDataSAP);
    FTParticipantInfo(InputStream& inputStream);
    virtual ~FTParticipantInfo();

    UUIDPtr& getUUID();
    
    SAPInfoPtr& getFTControlSAP();
    
    SAPInfoPtr& getFTDataSAP();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    UUIDPtr m_uuid;
    SAPInfoPtr m_ftControlSAP;
    SAPInfoPtr m_ftDataSAP;
};

typedef ACE_Strong_Bound_Ptr<FTParticipantInfo, ACE_SYNCH_MUTEX> FTParticipantInfoPtr;
#endif	/* FTPARTICIPANTINFO_H */

