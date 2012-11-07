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
 * File:   StartStreamReplyPacket.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 12:37 PM
 */

#ifndef STARTSTREAMREPLYPACKET_H
#define	STARTSTREAMREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>


class StartStreamReplyPacket : public SthenoStreamPacket {
public:
    static const UShort START_STREAM_REPLY_PACKET = 0x008888;
    StartStreamReplyPacket();
    StartStreamReplyPacket(InputStream& inputStream);    
    StartStreamReplyPacket(ULong packetID, ULong replyPacketID,Boolean returnStatus);
    virtual ~StartStreamReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    ULong getPacketReplyID();

protected:
    ULong m_replyPacketID;
    Boolean m_returnStatus;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* STARTSTREAMREPLYPACKET_H */

