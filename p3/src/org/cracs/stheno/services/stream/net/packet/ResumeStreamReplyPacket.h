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
 * File:   ResumeStreamReplyPacket.h
 * Author: rmartins
 *
 * Created on December 17, 2010, 12:55 PM
 */

#ifndef RESUMESTREAMREPLYPACKET_H
#define	RESUMESTREAMREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/services/stream/net/packet/StreamFramePacket.h>

class ResumeStreamReplyPacket : public SthenoStreamPacket {
public:
    static const UShort RESUME_STREAM_SERVICE_REPLY_PACKET_OP = 0x1477;

    ResumeStreamReplyPacket();
    ResumeStreamReplyPacket(InputStream& inputStream);
    ResumeStreamReplyPacket(ULong requestID, list<StreamFramePacket*>& frameList);
    virtual ~ResumeStreamReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    list<StreamFramePacket*>& getFrameList();

protected:
    list<StreamFramePacket*> m_frameList;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* RESUMESTREAMREPLYPACKET_H */

