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

#ifndef STREAMFRAMEPACKET_H
#define	STREAMFRAMEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class StreamFramePacket : public SthenoStreamPacket {
public:    
    static const UShort STREAM_FRAME_PACKET_OP = 0x7322;
    
    StreamFramePacket();
    StreamFramePacket(InputStream& inputStream);
    StreamFramePacket(ULong requestID,ULong frameNo,int frameSize,ACE_Time_Value& timestamp,ACE_Message_Block* payload);
    virtual ~StreamFramePacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    UUIDPtr& getUUID();
    UUIDPtr& getFID();
    int getFrameSize();
    ACE_Message_Block* getPayload();
    ULong getFrameNo();
    ACE_Time_Value& getTimestamp();

protected:
    ULong m_frameNo;
    int m_frameSize;
    ACE_Message_Block* m_payload;    
    ACE_Time_Value m_timestamp;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* STREAMFRAMEPACKET_H */

