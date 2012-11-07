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
 * File:   FramePacket.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 3:15 PM
 */

#ifndef FRAMEPACKET_H
#define	FRAMEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class FramePacket : public SthenoStreamPacket {
public:    
    static const UShort FRAME_PACKET_OP = 0x8322;
    
    FramePacket();
    FramePacket(const FramePacket& orig);    
    FramePacket(InputStream& inputStream);
    FramePacket(ULong requestID,ULong frameNo,int frameSize,ACE_Message_Block* payload);
    virtual ~FramePacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    int getFrameSize();
    ACE_Message_Block* getPayload();
    ULong getFrameNo();
    ACE_Time_Value& getTimestamp();
    
    FramePacket* duplicate();

protected:
    int m_frameSize;
    ACE_Message_Block* m_payload;    
    ULong m_frameNo;
    //stats
    ACE_Time_Value m_timestamp;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* FRAMEPACKET_H */

