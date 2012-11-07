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
 * File:   RequestCoordinatorReply.cpp
 * Author: rmartins
 * 
 * Created on September 15, 2010, 2:47 PM
 */

#include "RequestCoordinatorReply.h"

RequestCoordinatorReplyPacket::~RequestCoordinatorReplyPacket() {
}

RequestCoordinatorReplyPacket::RequestCoordinatorReplyPacket(
        const UUIDPtr& srcPID,
        const UUIDPtr& srcFID,
        const UUIDPtr& dstPID,
        const UUIDPtr& dstFID,
        ULong requestID,
        const UUIDPtr& uuid,
        const UUIDPtr& cellID,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& FTSAP,
        EndpointPtr& coordinatorEndpoint) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, REQUEST_COORDINATOR_REPLY_PACKET, requestID),
m_uuid(uuid), m_cellID(cellID), m_meshSAP(meshSAP),
m_discoverySAP(discoverySAP), m_FTSAP(FTSAP), m_coordinatorEndpoint(coordinatorEndpoint) {

}

RequestCoordinatorReplyPacket::RequestCoordinatorReplyPacket() : SthenoPacket() {
}

void
RequestCoordinatorReplyPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

UUIDPtr& RequestCoordinatorReplyPacket::getPID() {
    return m_uuid;
}

UUIDPtr& RequestCoordinatorReplyPacket::getCellID() {
    return m_cellID;
}

SAPInfoPtr& RequestCoordinatorReplyPacket::getMeshSAP() {
    return m_meshSAP;
}

SAPInfoPtr& RequestCoordinatorReplyPacket::getDiscoverySAP() {
    return m_discoverySAP;
}

SAPInfoPtr& RequestCoordinatorReplyPacket::getFTSAP() {
    return m_FTSAP;
}

EndpointPtr& RequestCoordinatorReplyPacket::getCoordinatorEndpoint() {
    return m_coordinatorEndpoint;
}

void
RequestCoordinatorReplyPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_uuid->serialize(outputStream);
    m_cellID->serialize(outputStream);
    if (!m_meshSAP.null()) {
        outputStream.write_boolean(true);
        m_meshSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    if (!m_discoverySAP.null()) {
        outputStream.write_boolean(true);
        m_discoverySAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    if (!m_FTSAP.null()) {
        outputStream.write_boolean(true);
        m_FTSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    if (!m_coordinatorEndpoint.null()) {
        outputStream.write_boolean(true);
        m_coordinatorEndpoint->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
}

void
RequestCoordinatorReplyPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* uuid = new UUID(inputStream);
    m_uuid.reset(uuid);
    UUID* cellID = new UUID(inputStream);
    m_cellID.reset(cellID);
    bool flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* meshSAP = new SAPInfo(inputStream);
        m_meshSAP.reset(meshSAP);
    } else {
        m_meshSAP.reset(0);
    }
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* discoverySAP = new SAPInfo(inputStream);
        m_discoverySAP.reset(discoverySAP);
    } else {
        m_discoverySAP.reset(0);
    }
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* ftSAP = new SAPInfo(inputStream);
        m_FTSAP.reset(ftSAP);
    } else {
        m_FTSAP.reset(0);
    }
    inputStream.read_boolean(flag);
    if (flag) {
        Endpoint* e = new Endpoint(inputStream);
        m_coordinatorEndpoint.reset(e);
    } else {
        m_coordinatorEndpoint.reset(0);
    }
}