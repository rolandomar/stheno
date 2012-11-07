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
 * File:   ResumeStreamServicePacket.h
 * Author: rmartins
 *
 * Created on December 17, 2010, 12:07 PM
 */

#ifndef RESUMESTREAMSERVICEPACKET_H
#define	RESUMESTREAMSERVICEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
//Frame size and rate is regulated by the service (specificed at creation time)

class ResumeStreamServicePacket : public SthenoStreamPacket {
public:
    static const UShort RESUME_STREAM_SERVICE_PACKET_OP = 0x1417;

    ResumeStreamServicePacket();
    ResumeStreamServicePacket(InputStream& inputStream);
    ResumeStreamServicePacket(ULong requestID, UUIDPtr& uuid, UUIDPtr& fid, UInt lastFrameNo);
    virtual ~ResumeStreamServicePacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    UInt getLastFrameNo();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UInt m_lastFrameNo;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* RESUMESTREAMSERVICEPACKET_H */

