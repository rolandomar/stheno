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

#ifndef ActuatorActionPacket_H
#define	ActuatorActionPacket_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class ActuatorActionPacket : public SthenoStreamPacket {
public:    
    static const UShort ACTION_PACKET_OP = 0x5555;
    
    ActuatorActionPacket();
    ActuatorActionPacket(InputStream& inputStream);
    ActuatorActionPacket(ULong requestID,ACE_Time_Value& timestamp,ACE_Message_Block* payload);
    virtual ~ActuatorActionPacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);


    ACE_Message_Block* getPayload();
    ACE_Time_Value& getTimestamp();

protected:
    ACE_Message_Block* m_payload;    
    ACE_Time_Value m_timestamp;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* ActuatorActionPacket_H */

