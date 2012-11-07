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
 * File:   P3MeshCreateServiceReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on October 1, 2010, 9:48 AM
 */

#include "P3MeshCreateServiceReplyPacket.h"

P3MeshCreateServiceReplyPacket::P3MeshCreateServiceReplyPacket() : m_exception(0), m_replyPacketID(0) {

}


P3MeshCreateServiceReplyPacket::P3MeshCreateServiceReplyPacket(InputStream& inputStream) :
m_exception(0),m_replyPacketID(0) {
    this->deserialize(inputStream);
}

P3MeshCreateServiceReplyPacket::P3MeshCreateServiceReplyPacket(UInt replyPacketID, UUIDPtr& iid) :
SthenoStreamPacket(P3MESH_CREATESERVICERPELY_PACKET, 0),
m_iid(iid), m_exception(0), m_replyPacketID(replyPacketID) {

}

P3MeshCreateServiceReplyPacket::P3MeshCreateServiceReplyPacket(UInt replyPacketID, MeshException* exception) :
SthenoStreamPacket(P3MESH_CREATESERVICERPELY_PACKET, 0),
m_exception(exception), m_replyPacketID(replyPacketID) {

}

P3MeshCreateServiceReplyPacket::~P3MeshCreateServiceReplyPacket() {
    if (m_exception != 0) {
        delete m_exception;
    }
}

bool P3MeshCreateServiceReplyPacket::isException() {
    return (m_exception != 0);
}

MeshException* P3MeshCreateServiceReplyPacket::getException() {
    return m_exception;
}

UUIDPtr& P3MeshCreateServiceReplyPacket::getIID() {
    return m_iid;
}

void
P3MeshCreateServiceReplyPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void P3MeshCreateServiceReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_replyPacketID);
    if (isException()) {
        outputStream.write_boolean(true);
        m_exception->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
        m_iid->serialize(outputStream);
    }
} 

void P3MeshCreateServiceReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_replyPacketID);
    bool flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        m_exception = new MeshException(inputStream);
        m_iid.reset(0);
    } else {
        UUID* iid = new UUID(inputStream);
        m_iid.reset(iid);
        if (m_exception != 0) {
            delete m_exception;
            m_exception = 0;
        }
    }
}
