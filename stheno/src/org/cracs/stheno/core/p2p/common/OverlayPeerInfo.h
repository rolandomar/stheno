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
 * File:   OverlayPeerInfo.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 5:42 PM
 */

#ifndef OVERLAYPEERINFO_H
#define	OVERLAYPEERINFO_H

#include <ace/Synch.h>
#include <ace/Refcounted_Auto_Ptr.h>

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/SAPInfo.h>
#include <euryale/serialization/Serializable.h>

class OverlayPeerInfo : public Serializable {
public:
    OverlayPeerInfo(UUIDPtr& uuid, UUIDPtr& fid, SAPInfoPtr& meshSAP, SAPInfoPtr& ftSAP, SAPInfoPtr& ftDataSAP);
    OverlayPeerInfo(UUIDPtr& uuid, UUIDPtr& fid, SAPInfoPtr& meshSAP, SAPInfoPtr& ftSAP);
    OverlayPeerInfo(InputStream& inputStream);
    OverlayPeerInfo(const OverlayPeerInfo& orig);
    virtual ~OverlayPeerInfo();

    UUIDPtr& getUUID();
    UUIDPtr& getFID();

    SAPInfoPtr& getMeshSAPInfo();

    SAPInfoPtr& getFTSAPInfo();

    bool hasFTDataSAP();
    void setFTDataSAP(SAPInfoPtr& ftDataSAP);
    SAPInfoPtr& getFTDataSAPInfo();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_ftSAP;
    SAPInfoPtr m_ftDataSAP;
};

typedef ACE_Strong_Bound_Ptr<OverlayPeerInfo, ACE_SYNCH_MUTEX> OverlayPeerInfoPtr;

#endif	/* OVERLAYPEERINFO_H */

