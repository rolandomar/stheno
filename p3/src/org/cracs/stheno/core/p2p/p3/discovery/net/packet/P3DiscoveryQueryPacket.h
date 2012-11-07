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
 * File:   P3DiscoveryQueryPacket.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 4:16 PM
 */

#ifndef P3DISCOVERYQUERYPACKET_H
#define	P3DISCOVERYQUERYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
class P3DiscoveryQueryPacket : public SthenoStreamPacket {
public:
    static const UShort P3DISCOVERY_QUERY_PACKET = 0x001111;
    P3DiscoveryQueryPacket();
    P3DiscoveryQueryPacket(InputStream& inputStream);
    P3DiscoveryQueryPacket(ULong packetID,UUIDPtr& uuid, UUIDPtr& fid,QoSEndpoint& qos,DiscoveryQueryPtr& query);
    P3DiscoveryQueryPacket(ULong packetID,UUIDPtr& uuid, UUIDPtr& fid,QoSEndpoint& qos,DiscoveryQuery* query);
    virtual ~P3DiscoveryQueryPacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    //Requester
    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    QoSEndpoint& getQoS();
    DiscoveryQueryPtr& getQuery();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    QoSEndpoint m_qos;
    DiscoveryQueryPtr m_query;

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};

#endif	/* P3DISCOVERYQUERYPACKET_H */

