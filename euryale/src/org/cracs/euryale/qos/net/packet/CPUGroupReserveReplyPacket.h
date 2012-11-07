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
 * File:   CPUGroupReserveReplyPacket.h
 * Author: rmartins
 *
 * Created on December 18, 2009, 3:31 PM
 */

#ifndef _CPUGROUPRESERVEREPLYPACKET_H
#define	_CPUGROUPRESERVEREPLYPACKET_H

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/Types.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

class CPUGroupReserveReplyPacket : public MedusaPacket {
public:
    //static const UInt CPUGROUP_RESERVE_REPLY_PACKET_ID = 0x000004;
    CPUGroupReserveReplyPacket(UInt requestID,UInt replyID,UInt reply);
    CPUGroupReserveReplyPacket(){}
    CPUGroupReserveReplyPacket(MedusaHeader* packetHeader) :
    MedusaPacket(packetHeader)
    {}

    virtual ~CPUGroupReserveReplyPacket();

    UInt getReply(){
        return m_reply;
    }

    UInt getRequestID(){
        return m_replyID;
    }

protected:
    UInt m_reply;
    UInt m_replyID;

    ACE_INLINE virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&){
        outputStream.write_ulong(m_replyID);
        outputStream.write_ulong(m_reply);
    }

    ACE_INLINE virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&){
        inputStream.read_ulong(m_replyID);
        inputStream.read_ulong(m_reply);

    }
};

#endif	/* _CPUGROUPRESERVEREPLYPACKET_H */

