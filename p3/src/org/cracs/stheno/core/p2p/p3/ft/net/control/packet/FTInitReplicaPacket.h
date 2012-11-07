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
 * File:   FTInitReplicaPacket.h
 * Author: rmartins
 *
 * Created on November 27, 2010, 3:00 PM
 */

#ifndef FTINITREPLICAPACKET_H
#define	FTINITREPLICAPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/core/p2p/common/OverlayPeerInfo.h>

class FTInitReplicaPacket : public SthenoStreamPacket {
public:
    static const UShort INIT_REPLICA_PACKET_OP = 0x7875;
    FTInitReplicaPacket();
    FTInitReplicaPacket(InputStream& inputStream);
    FTInitReplicaPacket(
            ULong requestID,
            UUIDPtr& uuid,
            UUIDPtr& fid,
            UUIDPtr& rgid,
            UUIDPtr& sid,
            UUIDPtr& iid,
            ServiceParamsPtr& params,
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& group
            );
    virtual ~FTInitReplicaPacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    UUIDPtr& getRGID();
    UUIDPtr& getSID();
    UUIDPtr& getIID();
    
    ServiceParamsPtr& getServiceParams();
    OverlayPeerInfoPtr& getPrimary();
    list<OverlayPeerInfoPtr>& getReplicationGroupMembers();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    ServiceParamsPtr m_params;
    OverlayPeerInfoPtr m_primary;
    list<OverlayPeerInfoPtr> m_group;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* FTINITREPLICAPACKET_H */

