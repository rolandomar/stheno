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
 * File:   RequestCellReplyPacket.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 27, 2010, 2:21 PM
 */

#include "RequestCellReplyPacket.h"

RequestCellReplyPacket::~RequestCellReplyPacket() {
}

RequestCellReplyPacket::RequestCellReplyPacket(UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID,
        //assigned cell
        CellAddrPtr& cell,
        UUIDPtr& coordinatorUUID,
        EndpointPtr& coordinatorEndpoint,
        //parent cell
        CellAddrPtr& parentCell,
        UUIDPtr& parentUUID,
        EndpointPtr& parentCoordinatorEndpoint,
        //SAP (either from cell's coordinator or from parent's cell coor.)
        SAPInfoPtr& discoverySAPInfo,
        SAPInfoPtr& meshSAPInfo,
        SAPInfoPtr& ftSAPInfo
        ) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, REQUEST_CELL_REPLY_PACKET, requestID),
m_cell(cell),
m_coordinatorUUID(coordinatorUUID),
m_coordinatorEndpoint(coordinatorEndpoint),
m_parentCell(parentCell),
m_parentCoordinatorEndpoint(parentCoordinatorEndpoint),
m_parentUUID(parentUUID),
m_discoverySAP(discoverySAPInfo),
m_meshSAP(meshSAPInfo),
m_ftSAPInfo(ftSAPInfo) {
}

RequestCellReplyPacket::RequestCellReplyPacket() : SthenoPacket() {
}

void
RequestCellReplyPacket::serialize(OutputStream& outputStream) THROW(SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

bool RequestCellReplyPacket::hasCoordinator() {
    return (!m_coordinatorEndpoint.null());
}

//cell related methods

CellAddrPtr& RequestCellReplyPacket::getCell() {
    return m_cell;
}

UUIDPtr& RequestCellReplyPacket::getCoordinatorUUID() {
    return m_coordinatorUUID;
}

EndpointPtr& RequestCellReplyPacket::getCoordinatorEndpoint() {
    return m_coordinatorEndpoint;
}
//Parent cell related methods

bool RequestCellReplyPacket::hasParentCoordinator() {
    return (!m_parentCoordinatorEndpoint.null());
}

bool RequestCellReplyPacket::hasParentCell() {
    return (!m_parentCell.null());
}

UUIDPtr& RequestCellReplyPacket::getParentUUID() {
    return m_parentUUID;
}

CellAddrPtr& RequestCellReplyPacket::getParentCell() {
    return m_parentCell;
}

EndpointPtr& RequestCellReplyPacket::getParentCoordinatorEndpoint() {
    return m_parentCoordinatorEndpoint;
}

SAPInfoPtr& RequestCellReplyPacket::getFTSAPInfo() {
    return m_ftSAPInfo;
}

SAPInfoPtr& RequestCellReplyPacket::getDiscoverySAPInfo() {
    return m_discoverySAP;
}

SAPInfoPtr& RequestCellReplyPacket::getMeshSAPInfo() {
    return m_meshSAP;
}

void
RequestCellReplyPacket::serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
    //cellID        
    m_cell->serialize(outputStream);
    if (!m_coordinatorUUID.null()) {
        outputStream.write_boolean(true);
        m_coordinatorUUID->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    //conditional coordinator serialization
    if (!m_coordinatorEndpoint.null()) {
        outputStream.write_boolean(true);
        m_coordinatorEndpoint->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    //conditional parentCelID serialization
    if (!m_parentCell.null()) {
        outputStream.write_boolean(true);
        m_parentCell->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
    //conditional parent coordinator serialization
    if (!m_parentUUID.null()) {
        outputStream.write_boolean(true);
        m_parentUUID->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    //conditional parent coordinator serialization
    if (!m_parentCoordinatorEndpoint.null()) {
        outputStream.write_boolean(true);
        m_parentCoordinatorEndpoint->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    if (!m_discoverySAP.null()) {
        outputStream.write_boolean(true);
        m_discoverySAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    if (!m_meshSAP.null()) {
        outputStream.write_boolean(true);
        m_meshSAP->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }

    if (!m_ftSAPInfo.null()) {
        outputStream.write_boolean(true);
        m_ftSAPInfo->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
}

void
RequestCellReplyPacket::deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
    Boolean serializationFlag = false;

    CellAddr* cell = new CellAddr(inputStream);
    m_cell.reset(cell);
    //cell endpoint
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        UUID* endpoint = new UUID(inputStream);
        m_coordinatorUUID.reset(endpoint);
    }

    //cell endpoint
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        Endpoint* endpoint = new Endpoint(inputStream);
        m_coordinatorEndpoint.reset(endpoint);
    }
    //parent cellid
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        CellAddr* parentCell = new CellAddr(inputStream);
        m_parentCell.reset(parentCell);
    }
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        UUID* endpoint = new UUID(inputStream);
        m_parentUUID.reset(endpoint);
    }
    //parent Endpoint
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        Endpoint* parentEndpoint = new Endpoint(inputStream);
        m_parentCoordinatorEndpoint.reset(parentEndpoint);
    }
    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        SAPInfo* sapInfo = new SAPInfo(inputStream);
        m_discoverySAP.reset(sapInfo);
    }

    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        SAPInfo* sapInfo = new SAPInfo(inputStream);
        m_meshSAP.reset(sapInfo);
    }

    inputStream.read_boolean(serializationFlag);
    if (serializationFlag) {
        SAPInfo* sapInfo = new SAPInfo(inputStream);
        m_ftSAPInfo.reset(sapInfo);
    }
}