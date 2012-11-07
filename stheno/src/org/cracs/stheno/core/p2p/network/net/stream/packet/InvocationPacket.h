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
 * File:   InvocationPacket.h
 * Author: rmartins
 *
 * Created on October 27, 2010, 12:26 PM
 */

#ifndef INVOCATIONPACKET_H
#define	INVOCATIONPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class MessagePacket : public SthenoStreamPacket {
public:
    static const UShort MESSAGE_PACKET_ID = 0x004352;
    MessagePacket();
    MessagePacket(InputStream& inputStream);
    MessagePacket(ULong packetID,ACE_Message_Block* mb);    
    virtual ~MessagePacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

protected:
    ACE_Message_Block* m_mb;

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* INVOCATIONPACKET_H */

