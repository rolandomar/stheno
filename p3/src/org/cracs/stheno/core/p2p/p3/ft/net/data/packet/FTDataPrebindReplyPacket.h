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
 * File:   FTDataPrebindReplyPacket.h
 * Author: rmartins
 *
 * Created on December 3, 2010, 11:34 AM
 */

#ifndef FTDATAPREBINDREPLYPACKET_H
#define	FTDATAPREBINDREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/SAPInfo.h>

class FTDataPrebindReplyPacket : public SthenoStreamPacket {
public:

    static const UShort FT_DATA_PREBIND_REPLY_PACKET_OP = 0x2300;
    FTDataPrebindReplyPacket();
    FTDataPrebindReplyPacket(InputStream& inputStream);
    FTDataPrebindReplyPacket(ULong packetID, UInt packetIDReply, bool acceptance);
    virtual ~FTDataPrebindReplyPacket();

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


#endif	/* FTDATAPREBINDREPLYPACKET_H */

