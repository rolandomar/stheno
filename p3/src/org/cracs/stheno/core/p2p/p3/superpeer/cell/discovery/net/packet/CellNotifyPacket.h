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
 * File:   CellNotifyPacket.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on July 30, 2010, 3:05 PM
 */

#ifndef CELLNOTIFYPACKET_H
#define	CELLNOTIFYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/P3DiscoveryPackets.h>

class CellNotifyPacket : public SthenoPacket {
public:

    CellNotifyPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        UInt   meshSize,
        ULong requestID):SthenoPacket(srcPID,srcFID,dstPID,dstFID,NOTIFY_PACKET,requestID),m_meshSize(meshSize){
    }
    //RequestCellPacket(){}
    CellNotifyPacket() :
    SthenoPacket(0){
    }
    

    virtual ~CellNotifyPacket();


    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&){
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*)m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized header!len=%lu\n", outputStream.total_length());
        serializeBody(outputStream);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)RequestCellPacket::serialize():serialized body!len=%lu\n", outputStream.total_length());
    }

    UInt getMeshSize(){
        return m_meshSize;
    }
protected:
    UInt m_meshSize;
    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&){          
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&){
    }
};

#endif	/* CELLNOTIFYPACKET_H */

