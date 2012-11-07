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
 * File:   ChangegIIDServiceUpdateInfo.h
 * Author: rmartins
 *
 * Created on January 11, 2011, 1:10 PM
 */

#ifndef CHANGEGIIDSERVICEUPDATEINFO_H
#define	CHANGEGIIDSERVICEUPDATEINFO_H

#include <stheno/core/p2p/p3/mesh/MeshUpdateInfo.h>

class ChangegIIDServiceUpdateInfo: public MeshUpdateInfo{
public:
    static const UInt UPDATE_IID_SERVICE_UPDATEINFO = 0x34899810;
    
    ChangegIIDServiceUpdateInfo(UUIDPtr& pid,UUIDPtr& fid,UUIDPtr& sid,UUIDPtr& oldIid,UUIDPtr& newIid);
    ChangegIIDServiceUpdateInfo(InfoUpdate* info,InputStream& inputStream);
    virtual ~ChangegIIDServiceUpdateInfo();
    
    UUIDPtr& getPID();
    UUIDPtr& getFID();
    UUIDPtr& getSID();
    UUIDPtr& getOldIID();
    UUIDPtr& getNewIID();
       
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
protected:
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    UUIDPtr m_sid;
    UUIDPtr m_oldIid;    
    UUIDPtr m_newIid;    
    
    virtual void serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&);
};



#endif	/* CHANGEGIIDSERVICEUPDATEINFO_H */

