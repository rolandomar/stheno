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
 * File:   PeerQuery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 13, 2010, 2:30 PM
 */

#ifndef SERVICEQUERY_H
#define	SERVICEQUERY_H

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
//#include <stheno/common/UID.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>

class ServiceQuery : public DiscoveryQuery {
public:
    static const UInt SERVICE_QUERY;
    ServiceQuery(UUIDPtr& sid);
    ServiceQuery(DiscoveryQuery* query);
    ServiceQuery(const ServiceQuery& orig);
    virtual ~ServiceQuery();

    UUIDPtr& getSID() {
        return m_sid;
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        deserializeMetadata(inputStream);
        UInt size = 0;
        inputStream.read_ulong(size);
        deserializeQuerySpecialization(inputStream);
    }

    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
        m_sid->serialize(outputStream);
    }

protected:
    UUIDPtr m_sid;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
        ByteInputStream inputStream(mb);
        deserializeQuerySpecialization(inputStream);
    }

    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
        UUID* uuid = new UUID(inputStream);
        m_sid.reset(uuid);
    }
};

#endif	/* SERVICEQUERY_H */


    /*virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        DiscoveryQuery::serialize(outputStream);
        m_sid->serialize(outputStream);
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        DiscoveryQuery::deserialize(inputStream);
        UUID* uuid = new UUID(inputStream);
        m_sid.reset(uuid);
    }

protected:
    UUIDPtr m_sid;*/