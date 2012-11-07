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
 * File:   SthenoStreamPacket.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 12:51 PM
 */

#include "SthenoStreamPacket.h"

SthenoStreamHeader::SthenoStreamHeader() :
m_magic(STHENO_STREAM_MAGIC), m_protocol(0), 
        //m_protocolVersionMajor(0),
//m_protocolVersionMinor(0),
        m_messageType(0),
m_requestID(0), m_messageSize(0) {
}

SthenoStreamHeader::SthenoStreamHeader(UShort messageType,
        ULong requestID, ULong messageSize) :
m_magic(STHENO_STREAM_MAGIC), m_protocol(0), //m_protocolVersionMajor(0),
//m_protocolVersionMinor(1),
m_messageType(messageType),
m_requestID(requestID),
 m_messageSize(messageSize) {
//m_messageSize(0) {

}

SthenoStreamHeader::SthenoStreamHeader(ULong magic,
        UShort protocol, //Byte protocolVersionMajor, Byte protocolVersionMinor,
        UShort messageType, ULong requestID,
        ULong messageSize) :
m_magic(magic), m_protocol(protocol),
//m_protocolVersionMajor(protocolVersionMajor),
//m_protocolVersionMinor(protocolVersionMinor),
m_messageType(messageType), m_requestID(requestID),
m_messageSize(messageSize) {
}

SthenoStreamHeader::~SthenoStreamHeader() {
}

SthenoStreamHeader::SthenoStreamHeader(InputStream& is) THROW(SerializationException&) {
    deserialize(is);
}

void SthenoStreamHeader::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    outputStream.write_ulong(m_magic);
    outputStream.write_ushort(m_protocol);
    //outputStream.write_octet(m_protocolVersionMajor);
    //outputStream.write_octet(m_protocolVersionMinor);
    outputStream.write_ushort(m_messageType);
    outputStream.write_ulong(m_requestID);
    outputStream.write_ulong(m_messageSize);
}

void SthenoStreamHeader::deserialize(InputStream& inputStream)
throw (SerializationException&) {
    inputStream.read_ulong(m_magic);
    inputStream.read_ushort(m_protocol);
    //inputStream.read_octet(m_protocolVersionMajor);
    //inputStream.read_octet(m_protocolVersionMinor);
    inputStream.read_ushort(m_messageType);
    inputStream.read_ulong(m_requestID);
    inputStream.read_ulong(m_messageSize);
}

UInt SthenoStreamHeader::getBodySize() {
    return m_messageSize;
}

void SthenoStreamHeader::setBodySize(UInt size) {
    m_messageSize = size;
}

UInt SthenoStreamHeader::getSize() {
    //return 18;
    return 16;
}

UInt SthenoStreamHeader::getSerializationSize(Alignment* aligment) {
    //return 18;
    return 16;
}

UShort SthenoStreamHeader::getType() {
    return m_messageType;
}

UInt SthenoStreamHeader::getPacketID() {
    return m_requestID;
}

UInt SthenoStreamHeader::getMessageSize() {
    return m_messageSize;
}

void SthenoStreamHeader::setMessageSize(UInt messageSize) {
    m_messageSize = messageSize;
}

void SthenoStreamHeader::setRequestID(UInt requestID) {
    m_requestID = requestID;
}

SthenoStreamPacket::SthenoStreamPacket(UShort messageType,
        ULong requestID,
        ULong messageSize) {
    SthenoStreamHeader* header = new SthenoStreamHeader(messageType,
            requestID, messageSize);
    Packet<SthenoStreamHeader>::m_packetHeader = header;
}

SthenoStreamPacket::SthenoStreamPacket(SthenoStreamHeader* packetHeader) :
Packet<SthenoStreamHeader>(packetHeader) {
}

SthenoStreamPacket::SthenoStreamPacket() : Packet<SthenoStreamHeader>() {
}

SthenoStreamPacket::~SthenoStreamPacket() {
}

void SthenoStreamPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    getPacketHeader()->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}