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
 * File:   UID.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 13, 2010, 4:42 PM
 */

#ifndef UID_H
#define	UID_H
/**
 * Universal identifier
 */

#include <euryale/common/uuid/UUID.h>
#include <euryale/serialization/Serializable.h>

class UID: public Serializable {
public:
    UID(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& sid);
    UID(const UID& orig);
    virtual ~UID();

    UUIDPtr& getPID() ;
    UUIDPtr& getFID() ;
    UUIDPtr& getSID();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_sid;    
};


#endif	/* UID_H */

