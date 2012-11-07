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
 * File:   P3MeshRemoveServiceReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on October 1, 2010, 9:49 AM
 */

#include "P3MeshRemoveServiceReplyPacket.h"
P3MeshRemoveServiceReplyPacket::P3MeshRemoveServiceReplyPacket():m_exception(0),m_replyPacketID(0) {
}
P3MeshRemoveServiceReplyPacket::P3MeshRemoveServiceReplyPacket(UInt replyPacketID):m_exception(0),m_replyPacketID(replyPacketID) {

}

P3MeshRemoveServiceReplyPacket::P3MeshRemoveServiceReplyPacket(InputStream& inputStream):
m_exception(0){
    this->deserialize(inputStream);
}

P3MeshRemoveServiceReplyPacket::P3MeshRemoveServiceReplyPacket(UInt replyPacketID,MeshException* exception) :
m_exception(exception),m_replyPacketID(replyPacketID) {

}

P3MeshRemoveServiceReplyPacket::~P3MeshRemoveServiceReplyPacket() {
    if (m_exception != 0) {
        delete m_exception;
    }
}

bool P3MeshRemoveServiceReplyPacket::isException() {
    return (m_exception!=0);
}

MeshException* P3MeshRemoveServiceReplyPacket::getException() {
    return m_exception;
}

void
P3MeshRemoveServiceReplyPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void P3MeshRemoveServiceReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&){
    outputStream.write_ulong(m_replyPacketID);
    if(isException()){
        outputStream.write_boolean(true);
        m_exception->serialize(outputStream);
    }else{
        outputStream.write_boolean(false);        
    }
}

void P3MeshRemoveServiceReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&){
    inputStream.read_ulong(m_replyPacketID);
    bool flag = false;
    inputStream.read_boolean(flag);    
    if(flag){        
        m_exception = new MeshException(inputStream);        
    }else{
    }
}