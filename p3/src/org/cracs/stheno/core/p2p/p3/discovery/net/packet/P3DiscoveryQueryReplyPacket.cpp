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
 * File:   P3DiscoveryQueryReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 4:17 PM
 */

#include "P3DiscoveryQueryReplyPacket.h"

P3DiscoveryQueryReplyPacket::P3DiscoveryQueryReplyPacket():
m_queryReply(0){
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryQueryReplyPacket:P3DiscoveryQueryReplyPacket() %@\n"), this));
}

P3DiscoveryQueryReplyPacket::P3DiscoveryQueryReplyPacket(InputStream& inputStream):
m_queryReply(0){
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryQueryReplyPacket:P3DiscoveryQueryReplyPacket() 2 %@\n"), this));
    this->deserialize(inputStream);
}

P3DiscoveryQueryReplyPacket::P3DiscoveryQueryReplyPacket(
        ULong packetID,
        ULong replyPacketID,
        UUIDPtr& uuid,
        UUIDPtr& fid,
        DiscoveryQueryReply* requeyReply) :
SthenoStreamPacket(P3DISCOVERY_QUERY_REPLY_PACKET, packetID),
m_replyPacketID(replyPacketID),
m_uuid(uuid),
m_fid(fid),
m_queryReply(requeyReply) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryQueryReplyPacket:P3DiscoveryQueryReplyPacket() 3 %@\n"), this));
}

P3DiscoveryQueryReplyPacket::~P3DiscoveryQueryReplyPacket() {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryQueryReplyPacket:~P3DiscoveryQueryReplyPacket() %@\n"), this));
    if (m_queryReply != 0) {
        delete m_queryReply;
    }
}

void
P3DiscoveryQueryReplyPacket::serialize(OutputStream& outputStream)
throw (SerializationException&) {
    //printf("P3DiscoveryQueryReplyPacket::serialize 1\n");
    UInt bodySize = getBodySerializationSize();
    //printf("P3DiscoveryQueryReplyPacket::serialize 2\n");
    m_packetHeader->setMessageSize(bodySize);
    //printf("P3DiscoveryQueryReplyPacket::serialize 3\n");
    serializeHeader(outputStream);
    //printf("P3DiscoveryQueryReplyPacket::serialize 4\n");
    serializeBody(outputStream);
    //printf("P3DiscoveryQueryReplyPacket::serialize 5\n");
}

UUIDPtr& P3DiscoveryQueryReplyPacket::getUUID() {
    return m_uuid;
}

UUIDPtr& P3DiscoveryQueryReplyPacket::getFID() {
    return m_fid;
}

DiscoveryQueryReply* P3DiscoveryQueryReplyPacket::getQuery() {
    return m_queryReply;
}

ULong P3DiscoveryQueryReplyPacket::getPacketReplyID() {
    return m_replyPacketID;

}

DiscoveryQueryReply* P3DiscoveryQueryReplyPacket::deAttachQuery() {
    DiscoveryQueryReply* temp = m_queryReply;
    m_queryReply = 0;
    return temp;
}

void
P3DiscoveryQueryReplyPacket::serializeBody(OutputStream& outputStream)
throw (SerializationException&) {
    //printf("P3DiscoveryQueryReplyPacket::serializeBody\n");
    outputStream.write_ulong(m_replyPacketID);
    //printf("P3DiscoveryQueryReplyPacket::serializeBody 1 - %p\n", m_uuid.get());
    m_uuid->serialize(outputStream);
    //printf("P3DiscoveryQueryReplyPacket::serializeBody 2 - %p\n", m_fid.get());
    m_fid->serialize(outputStream);
    //printf("P3DiscoveryQueryReplyPacket::serializeBody 3 - %p\n", m_queryReply);
    m_queryReply->serialize(outputStream);
    //printf("P3DiscoveryQueryReplyPacket::serializeBody 4\n");
}

void
P3DiscoveryQueryReplyPacket::deserializeBody(InputStream& inputStream)
throw (SerializationException&) {
    inputStream.read_ulong(m_replyPacketID);
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* fid = new UUID(inputStream);
    m_fid.reset(fid);
    if (m_queryReply != 0) {
        delete m_queryReply;
    }
    DiscoveryQueryReply* queryRelpy = new DiscoveryQueryReply(inputStream);
    m_queryReply = queryRelpy;

}


