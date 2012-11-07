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

#ifndef GETPARTITIONREPLY_H
#define	GETPARTITIONREPLY_H

#include <list>
using std::list;

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/Types.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUPartition.h>

class GetPartitionReplyPacket : public MedusaPacket {
public:
    //static const UInt GET_PARTITION_REPLY_PACKET_ID = 0x000003;
    GetPartitionReplyPacket(UInt replyID,list<CPUPartition*>* partitions);
    GetPartitionReplyPacket(UInt requestID,UInt replyID,list<CPUPartition*>* partitions);
    GetPartitionReplyPacket(){}
    GetPartitionReplyPacket(MedusaHeader* packetHeader) :
    MedusaPacket(packetHeader){
        
    }
    virtual ~GetPartitionReplyPacket();
    

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&){        
        UInt bodySize = getBodySerializationSize();        
        ((MedusaHeader*)m_packetHeader)->m_messageSize = bodySize;        
        serializeHeader(outputStream);        
        serializeBody(outputStream);       
    }

    list<CPUPartition*>& getCPUPartitions(){
        return m_partitions;
    }

    UInt getRequestID(){
        return m_replyID;
    }
protected:
    list<CPUPartition*> m_partitions;
    UInt m_replyID;
    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&){
        outputStream.write_ulong(m_replyID);
        outputStream.write_list<CPUPartition>(m_partitions);        
    }
    
    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)GetPartitionReplyPacket::deserializeBody\n")));
        inputStream.read_ulong(m_replyID);
        m_partitions.clear();
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)GetPartitionReplyPacket::deserializeBody before list\n")));
        inputStream.read_list<CPUPartition>(m_partitions);
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)GetPartitionReplyPacket::deserializeBody after list1\n")));
    }
};

#endif	/* _CPURESERVEPACKET_H */

