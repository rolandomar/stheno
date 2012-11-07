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
#ifndef PACKET_H_
#define PACKET_H_

#include /**/ <ace/pre.h>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <ace/Basic_Types.h>
#include <ace/Containers_T.h>

#include <euryale/common/Object.h>


#include <euryale/common/uuid/UUID.h>
//using pt::efacec::rto::runtime::uuid::UUID;

#include <euryale/serialization/Serializable.h>
#include <ace/Message_Block.h>
#include <euryale/common/io/RDRStream.h>

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>

#include <euryale/common/io/Alignment.h>

class PacketHeader {
public:

    PacketHeader() {}

    virtual ~PacketHeader() {}

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&) {}

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&) {}

    virtual UInt getSerializationSize(Alignment* aligment = 0) {
        return -1;
    }

    virtual UInt getBodySize() {
        return 0;
    }

};
//HEADER extends PacketHeader

template <class HEADER>
class Packet : public Object {
public:

    Packet(HEADER* packetHeader) :
    m_packetHeader(packetHeader) {
    }

    Packet() : m_packetHeader(0) {
    }

    virtual ~Packet() {
        if (m_packetHeader != 0)
            delete m_packetHeader;
    }       

    virtual void init(HEADER* header, InputStream& inputStream) THROW(SerializationException&) {
        /*ACE_Message_Block::Message_Flags flg = 0;
        ACE_Data_Block *db = 0;

        size_t rd_pos = incoming->rd_ptr() - incoming->base();
        size_t const wr_pos = incoming->wr_ptr() - incoming->base();

        // Get the flag in the packet block
        flg = incoming->self_flags();

        if (ACE_BIT_ENABLED(flg, ACE_Message_Block::DONT_DELETE)) {
            // Use the same datablock
            db = incoming->data_block();
        } else {serializeBody
            // Use a duplicated datablock as the datablock has come off the
            // heap.
            db = incoming->data_block()->duplicate();
        }*/
        m_packetHeader = header;
        deserializeBody(inputStream);
    }

    virtual const char* objectID() const {
        return "c27e96eba2d5952a3d3c30bea52a2f48";
    }

    ACE_INLINE virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&);

    virtual UInt getSerializationSize() {
        return this->m_packetHeader->getSerializationSize() + getBodySerializationSize();
    }

    virtual HEADER* getPacketHeader() {
        return m_packetHeader;
    }

    virtual UInt getBodySerializationSize() throw (SerializationException&) {
        StreamSize streamSize;
        serializeBody(streamSize);
        return streamSize.total_length();
    }

protected:
    HEADER* m_packetHeader;

    virtual void
    deserialize(InputStream& inputStream) THROW(SerializationException&) {
        throw new SerializationException(SerializationException::GENERAL_ERROR);
    }
    virtual void
    serializeHeader(OutputStream& outputStream) THROW(SerializationException&);

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&);

    virtual void
    deserializeBody(InputStream& outputStream) THROW(SerializationException&);
};

template <class HEADER>
void Packet<HEADER>::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) Packet::serialize()!\n")));
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

template <class HEADER> void Packet<HEADER>::serializeHeader(OutputStream& outputStream)
throw (SerializationException&) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) Packet::serializeHeader()!\n")));
    if (m_packetHeader != 0)
        m_packetHeader->serialize(outputStream);
    else
        throw new SerializationException(SerializationException::GENERAL_ERROR);
}

template <class HEADER> void Packet<HEADER>::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T) Packet::serializeBody()!\n")));
}

template <class HEADER> void Packet<HEADER>::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PAcket::HERE=\n")));
}


#include /**/ <ace/post.h>

#endif /*PACKET_H_*/

