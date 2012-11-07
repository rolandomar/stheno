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
 * File:   FindServiceInstanceQuery.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 7:22 PM
 */

#ifndef FINDSERVICEINSTANCEQUERY_H
#define	FINDSERVICEINSTANCEQUERY_H

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>

class FindServiceInstanceQuery : public DiscoveryQuery {
public:

    static const int FINDSERVICEINSTANCE_QUERY = 0x328300;

    FindServiceInstanceQuery(const UUIDPtr& sid, const UUIDPtr& iid);
    FindServiceInstanceQuery(DiscoveryQuery* query);
    FindServiceInstanceQuery(const FindServiceInstanceQuery& orig);
    virtual ~FindServiceInstanceQuery();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getSID();
    UUIDPtr& getIID();

protected:
    UUIDPtr m_sid;
    UUIDPtr m_iid;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* FINDSERVICEINSTANCEQUERY_H */

