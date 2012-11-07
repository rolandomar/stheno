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
 * File:   PeerQueryReply.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 13, 2010, 2:40 PM
 */

#ifndef PEERQUERYREPLY_H
#define	PEERQUERYREPLY_H

#include <list>
using std::list;

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

class PeerQueryReply: public DiscoveryQueryReply {
public:
    
    static const UInt ROUTING_QUERY_REPLY = 0xA1CF00;
    
    PeerQueryReply(PeerInfoPtr& info);
    PeerQueryReply(DiscoveryQueryReply* query);
    virtual ~PeerQueryReply();
   
    PeerInfoPtr& getPeerInfo();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
protected:
    PeerInfoPtr m_info;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);

};

#endif	/* PEERQUERYREPLY_H */

