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
 * File:   InitSessionPacket.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 1:22 PM
 */

#ifndef INITSESSIONPACKET_H
#define	INITSESSIONPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
class InitSessionPacket : public SthenoStreamPacket {
public:
    static const UShort INIT_SESSION_PACKET_OP = 0x000283;
    InitSessionPacket();
    InitSessionPacket(InputStream& inputStream);
    InitSessionPacket(ULong requestID,UUIDPtr& uuid, UUIDPtr& fid,QoSEndpoint& qos);
    virtual ~InitSessionPacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    QoSEndpoint& getQoS();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    QoSEndpoint m_qos;

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};

#endif	/* INITSESSIONPACKET_H */

