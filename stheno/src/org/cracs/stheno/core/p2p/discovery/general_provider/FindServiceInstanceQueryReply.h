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
 * File:   FindServiceInstanceQueryReply.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 7:22 PM
 */

#ifndef FINDSERVICEINSTANCEQUERYREPLY_H
#define	FINDSERVICEINSTANCEQUERYREPLY_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

class FindServiceInstanceQueryReply: public DiscoveryQueryReply {
public:
    
    static const UInt FINDSERVICEINSTANCE_QUERY_REPLY = 0x163130B;
    
    FindServiceInstanceQueryReply(UUIDPtr& sid,UUIDPtr& fid,SAPInfoPtr& sapInfo);
    FindServiceInstanceQueryReply(DiscoveryQueryReply* query);
    virtual ~FindServiceInstanceQueryReply();
   

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
    
    UUIDPtr& getSID();
    UUIDPtr& getFID();
    SAPInfoPtr& getSAPInfo();
    
protected:
    UUIDPtr m_sid;
    UUIDPtr m_fid;
    SAPInfoPtr m_sapInfo;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);   
};

#endif	/* FINDSERVICEINSTANCEQUERYREPLY_H */

