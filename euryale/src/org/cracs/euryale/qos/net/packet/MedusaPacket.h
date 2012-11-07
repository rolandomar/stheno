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
 * PacketBase.h
 *
 *  Created on: May 18, 2008
 *      Author: rmartins
 */

#ifndef HADESPACKET_H_
#define HADESPACKET_H_

#include <euryale/net/packet/Packet.h>
//#include <euryale/common/io/ObjectStreamFactory.h>
enum {
    GET_PARTITION_REQUEST_PACKET_ID,
    GET_PARTITION_REPLY_PACKET_ID,
    THREAD_RESERVE_REQUEST_PACKET_ID,
    THREAD_RESERVE_REPLY_PACKET_ID,
    CPUGROUP_RESERVE_REQUEST_PACKET_ID,
    CPUGROUP_RESERVE_REPLY_PACKET_ID,
    CLIENT_INIT_ID
            
};
namespace pt {

namespace efacec {

namespace rto {

namespace net {

namespace packet {




class MedusaHeader : public PacketHeader {
public:
    static const ULong RTO_MAGIC = 1381264720; // 'R' 'T' 'M' 'P'
    static const Byte RTO_MAJOR = 0;
    static const Byte RTO_MINOR = 1;
    static const Byte DEFAULT_FLAGS = 0;

    enum {
        NEW_MSG = 0x01, ACK_MSG = 0x02
    };

    MedusaHeader();
    MedusaHeader(ULong magic,
        UShort protocol,
        Byte protocolVersionMajor,
        Byte protocolVersionMinor,
        UShort messageType,
        ULong requestID,
        ULong messageSize = 0);

    MedusaHeader(UShort messageType,
        ULong requestID,
        ULong messageSize = 0);


    virtual ~MedusaHeader();

    MedusaHeader(InputStream& is) THROW(SerializationException&)
    {
        deserialize(is);
    }

    virtual void serialize(OutputStream& outputStream)
    THROW(SerializationException&)
    {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize\n")));
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize: magic=%d\n"), m_magic));
        outputStream.write_ulong(m_magic);
        outputStream.write_ushort(m_protocol);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize: m_protocol=%d\n"), m_protocol));
        outputStream.write_octet(m_protocolVersionMajor);
        outputStream.write_octet(m_protocolVersionMinor);
        outputStream.write_ushort(m_messageType);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize: m_messageType=%d\n"),
            //m_messageType));
        outputStream.write_ulong(m_requestID);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize: m_requestID=%d\n"), m_requestID));
        outputStream.write_ulong(m_messageSize);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::serialize: MessageSize=%d\n"),
            //m_messageSize));
    }

    /*
     * for future use
     *
     *
     */
    void deserialize(InputStream& inputStream)
    THROW(SerializationException&)
    {
        inputStream.read_ulong(m_magic);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::deserialize: magic=%d\n"), m_magic));
        inputStream.read_ushort(m_protocol);
        inputStream.read_octet(m_protocolVersionMajor);
        inputStream.read_octet(m_protocolVersionMinor);
        inputStream.read_ushort(m_messageType);
        inputStream.read_ulong(m_requestID);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::deserialize: m_requestID=%d\n"), m_requestID));
        inputStream.read_ulong(m_messageSize);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaHeader::deserialize: MessageSize=%d\n"),
            //m_messageSize));

    }

    UInt getBodySize()
    {
        return m_messageSize;
    }

    void setBodySize(UInt size)
    {
        m_messageSize = size;
    }

    static UInt getSize()
    {
        return 18;
    }

    virtual UInt getSerializationSize()
    {
        return getSize();
    }

    virtual UShort getType(){
        return m_messageType;
    }

    virtual UInt getPacketID(){
        return m_requestID;
    }

    UInt m_magic;
    UShort m_protocol; //Protocol number
    Byte m_protocolVersionMajor; //Protocol upper version number
    Byte m_protocolVersionMinor; //Protocol lower version number
    UShort m_messageType; //Packet type
    UInt m_requestID;

    UInt m_messageSize; //body size!

};

class MedusaPacket : public Packet<MedusaHeader> {
public:

    MedusaPacket(UShort messageType,
        ULong requestID,
        ULong messageSize = 0)
    {
        MedusaHeader* header = new MedusaHeader(messageType,
            requestID, messageSize);
        Packet<MedusaHeader>::m_packetHeader = header;
    }

    MedusaPacket(MedusaHeader* packetHeader) :
    Packet<MedusaHeader>(packetHeader)
    {
    }

    virtual ~MedusaPacket() {}
    
    PacketHeader* createPacketHeader() {
        return new MedusaHeader();
    }

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&)
    {
        //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) MedusaPacket::serialize()!\n")));
        UInt bodySize = getBodySerializationSize();
        //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) MedusaPacket::serialize(): after body size=%u!\n"), bodySize));
        ((MedusaHeader*) m_packetHeader)->m_messageSize = bodySize;
        //ACE_DEBUG((LM_DEBUG,
            //ACE_TEXT("(%T) MedusaPacket::serialize():serializing header!\n")));
        serializeHeader(outputStream);

        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaPacket::serialize():serialized header len=%lu!\n"), outputStream.total_length()));
        serializeBody(outputStream);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)MedusaPacket::serialize():serialized body!len=%lu\n"), outputStream.total_length()));
    }

protected:

    MedusaPacket()
    {
    }
};

}

}

}

}

}

using pt::efacec::rto::net::packet::MedusaPacket;
using pt::efacec::rto::net::packet::MedusaHeader;

#endif /* HADESPACKET_H_ */
