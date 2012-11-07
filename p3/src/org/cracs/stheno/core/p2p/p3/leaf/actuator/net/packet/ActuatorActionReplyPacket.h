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

#ifndef ActuatorActionReplyPacket_H
#define	ActuatorActionReplyPacket_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class ActuatorActionReplyPacket : public SthenoStreamPacket {
public:    
    static const UShort ACTION_REPLY_PACKET_OP = 0x1199;
    
    ActuatorActionReplyPacket();
    ActuatorActionReplyPacket(InputStream& inputStream);
    ActuatorActionReplyPacket(ULong requestID,ULong replyID,ACE_Time_Value& timestamp,bool acception);
    virtual ~ActuatorActionReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);
   
    ACE_Time_Value& getTimestamp();
    ULong getRequestID();
    bool getAcception();

protected:
    ULong m_requestID;
    bool m_acception;    
    ACE_Time_Value m_timestamp;
    
    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* ActuatorActionReplyPacket_H */

