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
 * File:   FramePacket.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 3:15 PM
 */

#include "ActionReplyPacket.h"

ActionReplyPacket::ActionReplyPacket():
m_acception(false)
{
}

ActionReplyPacket::ActionReplyPacket(InputStream& inputStream):
m_acception(false)
{
    this->deserialize(inputStream);
}

ActionReplyPacket::ActionReplyPacket(
        ULong packetID,ULong requestID,
        ACE_Time_Value& timestamp,
        bool acception
        ) :
SthenoStreamPacket(ACTION_REPLY_PACKET_OP, packetID),
m_acception(acception),m_timestamp(timestamp),m_requestID(requestID) {
}

ActionReplyPacket::~ActionReplyPacket() {    
}

void
ActionReplyPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    m_packetHeader->setMessageSize(bodySize);
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

ULong ActionReplyPacket::getRequestID(){
    return m_requestID;
}

bool ActionReplyPacket::getAcception() {
    return m_acception;
}

ACE_Time_Value& ActionReplyPacket::getTimestamp(){
    return m_timestamp;    
}

void
ActionReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {    
    outputStream.write_ulong(m_requestID);
    outputStream.write_longlong(m_timestamp.sec());
    outputStream.write_ulonglong(m_timestamp.usec());    
    outputStream.write_boolean(m_acception);
}

void
ActionReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_requestID);
    LongLong secs;
    ULongLong usecs;
    inputStream.read_longlong(secs);
    inputStream.read_ulonglong(usecs);
    m_timestamp.set(secs,usecs);
    inputStream.read_boolean(m_acception);
}
