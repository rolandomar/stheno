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
 * File:   SthenoPacket.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 27, 2010, 2:35 PM
 */

#ifndef _STHENOPACKET_H
#define	_STHENOPACKET_H
#include <euryale/net/packet/Packet.h>
#include <euryale/common/uuid/UUID.h>

class SthenoHeader : public PacketHeader {
public:
    static const ULong STHENO_MAGIC = 1381264720; // 'R' 'T' 'M' 'P'
    static const Byte STHENO_MAJOR = 0;
    static const Byte STHENO_MINOR = 1;
    static const Byte DEFAULT_FLAGS = 0;    

    SthenoHeader();
    SthenoHeader(ULong magic,
        UShort protocol,
        Byte protocolVersionMajor,
        Byte protocolVersionMinor,
        const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID,
        UShort messageType,
        ULong requestID,
        ULong messageSize = 0);

    SthenoHeader(const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID,
            UShort messageType,
        ULong requestID,
        ULong messageSize = 0);


    virtual ~SthenoHeader();

    SthenoHeader(InputStream& is) throw(SerializationException&);

    virtual void serialize(OutputStream& outputStream) throw(SerializationException&);    
    void deserialize(InputStream& inputStream) throw(SerializationException&);
    
    UInt getBodySize();

    void setBodySize(UInt size);

    static UInt getSize();
    
    virtual UInt getSerializationSize(Alignment* aligment = 0);
    
    virtual UShort getType();
    virtual UInt getPacketID();    
    virtual UUIDPtr& getSrcPID();
    virtual UUIDPtr& getDstPID();
    virtual UUIDPtr& getSrcFID();
    virtual UUIDPtr& getDstFID();
    
    virtual void setSrcPID(UUIDPtr& uuid);
    virtual void setSrcFID(UUIDPtr& uuid);
    virtual void setDstPID(UUIDPtr& uuid);
    virtual void setDstFID(UUIDPtr& uuid);
    virtual void setRequestID(UInt requestID);

    UInt m_magic;
    UShort m_protocol; //Protocol number
    Byte m_protocolVersionMajor; //Protocol upper version number
    Byte m_protocolVersionMinor; //Protocol lower version number
    // source peer's uuid
    UUIDPtr m_srcPID;
    // source peer's federation uuid
    UUIDPtr m_srcFID;
    // destination peer's uuid
    UUIDPtr m_dstPID;
    // destination peer's federation uuid
    UUIDPtr m_dstFID;
    UShort m_messageType; //Packet type
    UInt m_requestID;

    UInt m_messageSize; //body size!

};

class SthenoPacket : public Packet<SthenoHeader>{
public:

    SthenoPacket( const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID,UShort messageType,
        ULong requestID,
        ULong messageSize = 0);    
    
    SthenoPacket(SthenoHeader* packetHeader);
    SthenoPacket();    
    virtual ~SthenoPacket();
    
    virtual void serialize(OutputStream& outputStream) throw(SerializationException&);
};


#endif	/* _STHENOPACKET_H */

