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
 * File:   CellReply.cpp
 * Author: rmartins
 * 
 * Created on June 12, 2010, 2:44 PM
 */

#include "CellReply.h"

CellReply::CellReply(CellAddrPtr& cell, UUIDPtr& uuid, EndpointPtr& addr,
        CellAddrPtr& parentCell, UUIDPtr& parentUUID,
        EndpointPtr& parentAddr, SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP)
:
m_cell(cell),
m_addr(addr),
m_uuid(uuid),
m_parentUUID(parentUUID),
m_parentCell(parentCell),
m_parentAddr(parentAddr),
m_discoverySAP(discoverySAP),
m_meshSAP(meshSAP),
m_ftSAP(ftSAP) {

}

CellReply::CellReply(CellAddrPtr& cell, CellAddrPtr& parentCell,
        UUIDPtr& parentUUID, EndpointPtr& parentAddr, SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP) :
m_cell(cell),
m_parentUUID(parentUUID),
m_parentCell(parentCell),
m_parentAddr(parentAddr),
m_discoverySAP(discoverySAP),
m_meshSAP(meshSAP),
m_ftSAP(ftSAP) {
}

CellReply::CellReply(CellAddrPtr& cell, UUIDPtr& uuid, EndpointPtr& addr,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP) :
m_uuid(uuid),
m_cell(cell),
m_addr(addr),
m_discoverySAP(discoverySAP),
m_meshSAP(meshSAP),
m_ftSAP(ftSAP) {
}

CellReply::CellReply(CellAddrPtr& cell, SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP, SAPInfoPtr& ftSAP) :
m_cell(cell), m_discoverySAP(discoverySAP),
m_meshSAP(meshSAP), m_ftSAP(ftSAP) {
}

CellReply::~CellReply() {
}

bool CellReply::hasCoordinator() {
    return (!m_addr.null());
}

UUIDPtr& CellReply::getCoordinatorUUID() {
    return m_uuid;
}

CellAddrPtr& CellReply::getCell() {
    return m_cell;
}

EndpointPtr& CellReply::getCoordinatorAddr() {
    return m_addr;
}

bool CellReply::hasParentCoordinator() {
    return (!m_parentAddr.null());
}

bool CellReply::hasParentCellID() {
    return (!m_parentCell.null());
}

CellAddrPtr& CellReply::getParentCell() {
    return m_parentCell;
}

EndpointPtr& CellReply::getParentCoordinatorAddr() {
    return m_parentAddr;
}

UUIDPtr& CellReply::getParentUUID() {
    return m_parentUUID;
}

SAPInfoPtr& CellReply::getDiscoverySAPInfo() {
    return m_discoverySAP;
}

SAPInfoPtr& CellReply::getFTSAPInfo() {
    return m_ftSAP;
}

SAPInfoPtr& CellReply::getMeshSAPInfo() {
    return m_meshSAP;
}