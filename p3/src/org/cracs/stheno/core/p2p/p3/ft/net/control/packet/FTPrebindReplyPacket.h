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
 * File:   FTPrebindReplyPacket.h
 * Author: rmartins
 *
 * Created on December 2, 2010, 5:12 PM
 */

#ifndef FTPREBINDREPLYPACKET_H
#define	FTPREBINDREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/SAPInfo.h>

class FTPrebindReplyPacket : public SthenoStreamPacket {
public:

    static const UShort PREBIND_REPLY_PACKET_OP = 0x1300;
    FTPrebindReplyPacket();
    FTPrebindReplyPacket(InputStream& inputStream);
    FTPrebindReplyPacket(ULong packetID, UInt packetIDReply, bool acceptance);
    virtual ~FTPrebindReplyPacket();

    virtual void serialize(OutputStream& outputStream) THROW(SerializationException&);

    bool getAcceptanceValue();
    ULong getPacketReplyID();

protected:
    ULong m_packetIDReply;
    bool m_acceptance;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&);
};


#endif	/* FTPREBINDREPLYPACKET_H */

