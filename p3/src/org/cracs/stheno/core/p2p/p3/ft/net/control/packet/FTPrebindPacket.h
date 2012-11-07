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
 * File:   FTPrebindPacket.h
 * Author: rmartins
 *
 * Created on December 2, 2010, 4:55 PM
 */

#ifndef FTPREBINDPACKET_H
#define	FTPREBINDPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/core/p2p/common/OverlayPeerInfo.h>

class FTPrebindPacket : public SthenoStreamPacket {
public:
    static const UShort PREBIND_PACKET_OP = 0x1350;
    FTPrebindPacket();
    FTPrebindPacket(InputStream& inputStream);
    FTPrebindPacket(
            ULong requestID,
            UUIDPtr& uuid,
            UUIDPtr& fid,
            UUIDPtr& rgid);
    virtual ~FTPrebindPacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    UUIDPtr& getRGID();        

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* FTPREBINDPACKET_H */

