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
 * File:   P3DiscoveryQueryReplyPacket.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 4:17 PM
 */

#ifndef P3DISCOVERYQUERYREPLYPACKET_H
#define	P3DISCOVERYQUERYREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>

class P3DiscoveryQueryReplyPacket : public SthenoStreamPacket {
public:
    static const UShort P3DISCOVERY_QUERY_REPLY_PACKET = 0x001112;
    P3DiscoveryQueryReplyPacket();
    P3DiscoveryQueryReplyPacket(InputStream& inputStream);
    //P3DiscoveryQueryReplyPacket(ULong packetID, ULong replyPacketID,UUIDPtr& uuid, UUIDPtr& fid, DiscoveryQueryReplyPtr& requeyReply);
    P3DiscoveryQueryReplyPacket(ULong packetID, ULong replyPacketID,UUIDPtr& uuid, UUIDPtr& fid, DiscoveryQueryReply* requeyReply);
    virtual ~P3DiscoveryQueryReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    //Requester
    UUIDPtr& getUUID();
    UUIDPtr& getFID();    
    ULong getPacketReplyID();
    DiscoveryQueryReply* getQuery();
    DiscoveryQueryReply* deAttachQuery();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;    
    ULong m_replyPacketID;
    DiscoveryQueryReply* m_queryReply;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* P3DISCOVERYQUERYREPLYPACKET_H */

