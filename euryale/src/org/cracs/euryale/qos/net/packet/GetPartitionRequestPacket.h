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
 * File:   CPUReservePacket.h
 * Author: rmartins
 *
 * Created on December 17, 2009, 12:49 PM
 */

#ifndef GETPARTITIONREQUEST_H
#define	GETPARTITIONREQUEST_H

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/Types.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

class GetPartitionRequestPacket : public MedusaPacket {
public:
    //static const UInt GET_PARTITION_REQUEST_PACKET_ID = 0x000002;
    
    GetPartitionRequestPacket(UInt requestID = 0);    
    GetPartitionRequestPacket(MedusaHeader* packetHeader);
    virtual ~GetPartitionRequestPacket();
    

    virtual void serialize(OutputStream& outputStream) throw(SerializationException&);
    
protected:
    virtual void serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void deserializeBody(InputStream& inputStream) throw(SerializationException&);
};

#endif	/* _CPURESERVEPACKET_H */

