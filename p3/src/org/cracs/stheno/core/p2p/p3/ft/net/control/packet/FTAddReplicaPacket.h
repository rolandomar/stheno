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
 * File:   FTAddReplicaPacket.h
 * Author: rmartins
 *
 * Created on November 27, 2010, 1:13 PM
 */

#ifndef FTADDREPLICAPACKET_H
#define	FTADDREPLICAPACKET_H


#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/core/p2p/common/OverlayPeerInfo.h>

class FTAddReplicaPacket : public SthenoStreamPacket {
public:
    static const UShort ADD_REPLICA_PACKET_OP = 0x7873;
    FTAddReplicaPacket();
    FTAddReplicaPacket(InputStream& inputStream);
    FTAddReplicaPacket(
            ULong requestID,
            UUIDPtr& uuid,
            UUIDPtr& fid,
            UUIDPtr& rgid,
            OverlayPeerInfoPtr& peer);
    virtual ~FTAddReplicaPacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    UUIDPtr& getRGID();
    OverlayPeerInfoPtr& getPeerInfo();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;
    OverlayPeerInfoPtr m_peer;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* FTADDREPLICAPACKET_H */

