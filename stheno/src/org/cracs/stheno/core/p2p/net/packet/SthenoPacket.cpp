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
 * File:   SthenoPacket.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 27, 2010, 2:35 PM
 */

#include "SthenoPacket.h"

SthenoHeader::SthenoHeader() :
m_magic(STHENO_MAGIC), m_protocol(0), m_protocolVersionMajor(0),
m_protocolVersionMinor(0), m_messageType(0),
m_requestID(0), m_messageSize(0) {
}

SthenoHeader::SthenoHeader(const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID, UShort messageType,
        ULong requestID, ULong messageSize) :
m_magic(STHENO_MAGIC), m_protocol(0), m_protocolVersionMajor(0),
m_protocolVersionMinor(1),
m_srcPID(srcPID), m_srcFID(srcFID), m_dstPID(dstPID), m_dstFID(dstFID),
m_messageType(messageType),
m_requestID(requestID),
m_messageSize(0) {

}

SthenoHeader::SthenoHeader(ULong magic,
        UShort protocol, Byte protocolVersionMajor, Byte protocolVersionMinor,
        const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID,
        UShort messageType, ULong requestID,
        ULong messageSize) :
m_magic(magic), m_protocol(protocol),
m_protocolVersionMajor(protocolVersionMajor),
m_protocolVersionMinor(protocolVersionMinor),
m_srcPID(srcPID), m_srcFID(srcFID), m_dstPID(srcPID), m_dstFID(dstFID),
m_messageType(messageType), m_requestID(requestID),
m_messageSize(messageSize) {
}

SthenoHeader::~SthenoHeader() {
}

SthenoHeader::SthenoHeader(InputStream& is) throw (SerializationException&) {
    deserialize(is);
}

void SthenoHeader::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize\n");
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: magic=%d %lu\n", m_magic,outputStream.total_length());
    outputStream.write_ulong(m_magic);
    outputStream.write_ushort(m_protocol);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: m_protocol=%d %lu\n", m_protocol,outputStream.total_length());
    outputStream.write_octet(m_protocolVersionMajor);
    outputStream.write_octet(m_protocolVersionMinor);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: m_protocolVersionMajor=%d %lu\n", m_protocolVersionMajor,outputStream.total_length());
    m_srcPID->serialize(outputStream);
    m_srcFID->serialize(outputStream);
    m_dstPID->serialize(outputStream);
    m_dstFID->serialize(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: (%s,%s) (%s,%s)\n",
    //m_srcPID->toString().c_str(),m_srcFID->toString().c_str(),
    //m_dstPID->toString().c_str(),m_dstFID->toString().c_str());
    outputStream.write_ushort(m_messageType);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: m_messageType=%d %lu\n",
    //m_messageType,outputStream.total_length());
    outputStream.write_ulong(m_requestID);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: m_requestID=%d %lu\n", m_requestID,outputStream.total_length());
    outputStream.write_ulong(m_messageSize);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::serialize: MessageSize=%d %lu\n",
    //m_messageSize,outputStream.total_length());
}

/*
 * for future use
 *
 *
 */
void SthenoHeader::deserialize(InputStream& inputStream)
throw (SerializationException&) {
    inputStream.read_ulong(m_magic);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: magic=%d buffer_size=%l\n"), m_magic,inputStream.length()));
    inputStream.read_ushort(m_protocol);
    inputStream.read_octet(m_protocolVersionMajor);
    inputStream.read_octet(m_protocolVersionMinor);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: before SRC_PID buffer_size(%l)\n"),inputStream.length()));
    UUID* srcPID = new UUID();
    srcPID->deserialize(inputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: after SRC_PID buffer_size(%l)\n"),inputStream.length()));
    m_srcPID.reset(srcPID);

    UUID* srcFID = new UUID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: before SRC_FID buffer_size=%l\n"),inputStream.length()));
    srcFID->deserialize(inputStream);
    m_srcFID.reset(srcFID);
    UUID* dstPID = new UUID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: before DST_PID buffer_size=%l\n"),inputStream.length()));
    dstPID->deserialize(inputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: after DST_PID buffer_size=%l\n"),inputStream.length()));
    m_dstPID.reset(dstPID);

    UUID* dstFID = new UUID();
    dstFID->deserialize(inputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: before DST_FID buffer_size=%l\n"),inputStream.length()));
    m_dstFID.reset(dstFID);


    /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: (%s,%s) (%s,%s) buffer_size=%lu\n",
    srcPID->toString().c_str(),srcFID->toString().c_str(),
    dstPID->toString().c_str(),dstFID->toString().c_str(),inputStream.length());*/

    inputStream.read_ushort(m_messageType);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: type(%d) buffer_size(%lu)\n",
    //      m_messageType,inputStream.length());
    inputStream.read_ulong(m_requestID);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: m_requestID=%d buffer_size=%lu\n", m_requestID,inputStream.length());
    inputStream.read_ulong(m_messageSize);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: MessageSize=%d buffer_size=%lu\n",
    //m_messageSize,inputStream.length());
    /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoHeader::deserialize: type(%d) request_id(%d) size(%d) (%s,%s) (%s,%s) buffer_size=%lu\n",
    m_messageType,m_requestID,m_messageSize,
    srcPID->toString().c_str(),srcFID->toString().c_str(),
    dstPID->toString().c_str(),dstFID->toString().c_str(),inputStream.length());*/
}

UInt SthenoHeader::getBodySize() {
    return m_messageSize;
}

void SthenoHeader::setBodySize(UInt size) {
    m_messageSize = size;
}

UInt SthenoHeader::getSize() {
    //return 82;
    return 98;
}

UInt SthenoHeader::getSerializationSize(Alignment* aligment) {
    //return 82;
    return 98;
}

/*virtual UInt getSerializationSize()
{
    return getSize();
}*/

UShort SthenoHeader::getType() {
    return m_messageType;
}

UInt SthenoHeader::getPacketID() {
    return m_requestID;
}

UUIDPtr& SthenoHeader::getSrcPID() {
    return m_srcPID;
}

UUIDPtr& SthenoHeader::getDstPID() {
    return m_dstPID;
}

UUIDPtr& SthenoHeader::getSrcFID() {
    return m_srcFID;
}

UUIDPtr& SthenoHeader::getDstFID() {
    return m_dstFID;
}

void SthenoHeader::setSrcPID(UUIDPtr& uuid) {
    m_srcPID = uuid;
}

void SthenoHeader::setSrcFID(UUIDPtr& uuid) {
    m_srcFID = uuid;
}

void SthenoHeader::setDstPID(UUIDPtr& uuid) {
    m_dstPID = uuid;
}

void SthenoHeader::setDstFID(UUIDPtr& uuid) {
    m_dstFID = uuid;
}

void SthenoHeader::setRequestID(UInt requestID) {
    m_requestID = requestID;
}

SthenoPacket::SthenoPacket(
const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID, UShort messageType,
        ULong requestID,
        ULong messageSize) {
    SthenoHeader* header = new SthenoHeader(srcPID, srcFID, dstPID, dstFID, messageType,
            requestID, messageSize);
    Packet<SthenoHeader>::m_packetHeader = header;
}

SthenoPacket::SthenoPacket(SthenoHeader* packetHeader) : Packet<SthenoHeader>(packetHeader) {
}

SthenoPacket::SthenoPacket() : Packet<SthenoHeader>() {
}

SthenoPacket::~SthenoPacket() {
}

void
SthenoPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    //ACE_DEBUG((LM_DEBUG,
    //ACE_TEXT("(%T) SthenoPacket::serialize()!\n")));
    UInt bodySize = getBodySerializationSize();
    //ACE_DEBUG((LM_DEBUG,
    //ACE_TEXT("(%T) SthenoPacket::serialize(): after body size=%u!\n"), bodySize));
    getPacketHeader()->m_messageSize = bodySize;
    /*ACE_DEBUG((LM_DEBUG,
        ACE_TEXT("(%T) SthenoPacket::serialize():serializing header!\n")));*/
    serializeHeader(outputStream);

    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoPacket::serialize():serialized header len=%lu!\n", outputStream.total_length());
    serializeBody(outputStream);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)SthenoPacket::serialize():serialized body!len=%lu\n", outputStream.total_length());
}