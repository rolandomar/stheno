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
 * File:   CoordinatorReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on August 24, 2010, 2:19 PM
 */

#include "CoordinatorReplyPacket.h"

CoordinatorReplyPacket::CoordinatorReplyPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID,
        EndpointPtr& coordinatorEndpoint) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, FIND_COORDINATOR_REPLY_PACKET, requestID),
m_coordinatorEndpoint(coordinatorEndpoint) {

}

CoordinatorReplyPacket::CoordinatorReplyPacket() : SthenoPacket() {
}

CoordinatorReplyPacket::~CoordinatorReplyPacket() {
}

void
CoordinatorReplyPacket::serialize(OutputStream& outputStream) throw(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

EndpointPtr& CoordinatorReplyPacket::getCoordinatorEndpoint() {
    return m_coordinatorEndpoint;
}

void
CoordinatorReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_coordinatorEndpoint->serialize(outputStream);
}

void
CoordinatorReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    Endpoint* endpoint = new Endpoint(inputStream);
    m_coordinatorEndpoint.reset(endpoint);
}
