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
 * File:   ResumeStreamReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on December 17, 2010, 12:55 PM
 */

#include "ResumeStreamReplyPacket.h"
#include <euryale/common/ListHelper.h>

ResumeStreamReplyPacket::ResumeStreamReplyPacket() {
}

ResumeStreamReplyPacket::ResumeStreamReplyPacket(InputStream& inputStream) {
    this->deserialize(inputStream);
}

ResumeStreamReplyPacket::ResumeStreamReplyPacket(
        ULong requestID,
        list<StreamFramePacket*>& frameList
        ) :
SthenoStreamPacket(RESUME_STREAM_SERVICE_REPLY_PACKET_OP, requestID),
m_frameList(frameList) {
}

ResumeStreamReplyPacket::~ResumeStreamReplyPacket() {
    ListHelper<StreamFramePacket*>::clearList(m_frameList);
}

void
ResumeStreamReplyPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void
ResumeStreamReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    UInt size = m_frameList.size();
    outputStream.write_ulong(size);
    list<StreamFramePacket*>::iterator iter = m_frameList.begin();
    while (iter != m_frameList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
}

void
ResumeStreamReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UInt size = 0;
    inputStream.read_ulong(size);
    for (int i = 0; i < size; i++) {
        SthenoStreamHeader* header = new SthenoStreamHeader(inputStream);
        StreamFramePacket* packet = new StreamFramePacket();
        packet->init(header,inputStream);
        m_frameList.push_back(packet);
    }
}

list<StreamFramePacket*>& ResumeStreamReplyPacket::getFrameList() {
    return m_frameList;
}