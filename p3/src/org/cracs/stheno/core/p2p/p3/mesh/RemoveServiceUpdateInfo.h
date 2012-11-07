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
 * File:   RemoveServiceUpdateInfo.h
 * Author: rmartins
 *
 * Created on January 11, 2011, 1:08 PM
 */

#ifndef REMOVESERVICEUPDATEINFO_H
#define	REMOVESERVICEUPDATEINFO_H

#include <stheno/core/p2p/p3/mesh/MeshUpdateInfo.h>

class RemoveServiceUpdateInfo: public MeshUpdateInfo{
public:
    static const UInt REMOVE_SERVICE_UPDATEINFO = 0x34899812;
    
    RemoveServiceUpdateInfo(UUIDPtr& pid,UUIDPtr& fid,UUIDPtr& sid,UUIDPtr& iid);
    RemoveServiceUpdateInfo(InfoUpdate* info,InputStream& inputStream);
    virtual ~RemoveServiceUpdateInfo();
    
    UUIDPtr& getPID();
    UUIDPtr& getFID();
    UUIDPtr& getSID();
    UUIDPtr& getIID();
       
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
protected:
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    UUIDPtr m_sid;
    UUIDPtr m_iid;    
    
    virtual void serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* REMOVESERVICEUPDATEINFO_H */

