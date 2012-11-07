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
 * File:   PeerLocation.h
 * Author: rmartins
 *
 * Created on September 22, 2010, 4:48 PM
 */

#ifndef PEERLOCATION_H
#define	PEERLOCATION_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/serialization/Serializable.h>

class PeerLocation {
public:
    PeerLocation(const UUIDPtr& uuid, const UUIDPtr& fid);
    PeerLocation(const PeerLocation& orig);
    virtual ~PeerLocation();

    UUIDPtr& getPID() {
        return m_uuid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        m_uuid->serialize(outputStream);
        m_fid->serialize(outputStream);
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        UUID* uuid = new UUID(inputStream);
        m_uuid.reset(uuid);
        UUID* fid = new UUID(inputStream);
        m_fid.reset(fid);
    }

    bool operator==(const PeerLocation& uuid) const {                
        return isEqual(uuid);
    }

    bool isEqual(const PeerLocation& uuid) const {        
       return ((m_uuid->isEqual(uuid.m_uuid)) && (m_fid->isEqual(uuid.m_fid)));        
    }

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;

};

#endif	/* PEERLOCATION_H */

