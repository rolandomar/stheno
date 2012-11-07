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
 * File:   SthenoStreamPacket.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 12:51 PM
 */

#ifndef STHENOSTREAMPACKET_H
#define	STHENOSTREAMPACKET_H

#include <euryale/net/packet/Packet.h>
#include <euryale/common/uuid/UUID.h>

class SthenoStreamHeader : public PacketHeader {
public:
    static const ULong STHENO_STREAM_MAGIC = 907128951;
    //static const Byte STHENO_MAJOR = 0;
   // static const Byte STHENO_MINOR = 1;
    static const Byte DEFAULT_FLAGS = 0;

    SthenoStreamHeader();
    SthenoStreamHeader(ULong magic,
            UShort protocol,
           // Byte protocolVersionMajor,
            //Byte protocolVersionMinor,
            UShort messageType,
            ULong requestID,
            ULong messageSize = 0);

    SthenoStreamHeader(UShort messageType,
            ULong requestID,
            ULong messageSize = 0);

    virtual ~SthenoStreamHeader();

    SthenoStreamHeader(InputStream& is) THROW(SerializationException&);

    virtual void serialize(OutputStream& outputStream) throw(SerializationException&);
    void deserialize(InputStream& inputStream)throw(SerializationException&);

    UInt getBodySize();
    void setBodySize(UInt size);
    
    static UInt getSize();

    virtual UInt getSerializationSize(Alignment* aligment = 0);

    virtual UShort getType();

    virtual UInt getPacketID();

    virtual UInt getMessageSize();

    virtual void setMessageSize(UInt messageSize);

    virtual void setRequestID(UInt requestID);
    
protected:
    UInt m_magic;
    UShort m_protocol; //Protocol number
    //Byte m_protocolVersionMajor; //Protocol upper version number
    //Byte m_protocolVersionMinor; //Protocol lower version number
    UShort m_messageType;
    UInt m_requestID;
    UInt m_messageSize;

};

class SthenoStreamPacket : public Packet<SthenoStreamHeader> {
public:

    SthenoStreamPacket(UShort messageType,
            ULong requestID,
            ULong messageSize = 0);

    SthenoStreamPacket(SthenoStreamHeader* packetHeader);
    SthenoStreamPacket();
    virtual ~SthenoStreamPacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
};



#endif	/* STHENOSTREAMPACKET_H */

