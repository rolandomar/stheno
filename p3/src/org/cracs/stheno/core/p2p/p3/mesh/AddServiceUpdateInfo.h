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
 * File:   AddServiceUpdateInfo.h
 * Author: rmartins
 *
 * Created on January 11, 2011, 1:05 PM
 */

#ifndef ADDSERVICEUPDATEINFO_H
#define	ADDSERVICEUPDATEINFO_H

#include <stheno/core/p2p/p3/mesh/MeshUpdateInfo.h>

class AddServiceUpdateInfo: public MeshUpdateInfo{
public:
    static const UInt ADD_SERVICE_UPDATEINFO = 0x21389021;
    
    AddServiceUpdateInfo(UUIDPtr& pid,UUIDPtr& fid,ServiceInstanceInfoPtr& info);
    AddServiceUpdateInfo(InfoUpdate* info,InputStream& inputStream);
    virtual ~AddServiceUpdateInfo();
    
    UUIDPtr& getPID();
    UUIDPtr& getFID();
    ServiceInstanceInfoPtr& getServiceInfo();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
protected:
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    ServiceInstanceInfoPtr m_info;
    
    virtual void serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* ADDSERVICEUPDATEINFO_H */

