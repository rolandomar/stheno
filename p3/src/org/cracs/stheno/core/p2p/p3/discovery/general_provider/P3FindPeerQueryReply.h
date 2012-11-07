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
 * File:   FindQueryReply.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 7:09 PM
 */

#ifndef P3FINDQUERYREPLY_H
#define	P3FINDQUERYREPLY_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

class P3FindPeerQueryReply : public DiscoveryQueryReply {
public:

    static const Int P3FINDPEER_QUERY_REPLY = 0xBC311CC;

    P3FindPeerQueryReply(PeerInfoPtr& pi);
    P3FindPeerQueryReply(DiscoveryQueryReply* query);
    virtual ~P3FindPeerQueryReply();


    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);

    PeerInfoPtr& getPeerInfo();

protected:
    PeerInfoPtr m_pi;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* P3FINDQUERYREPLY_H */

