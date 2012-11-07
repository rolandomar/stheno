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
 * File:   CoordinatorInfo.cpp
 * Author: rmartins
 * 
 * Created on January 11, 2011, 12:45 PM
 */

#include "CoordinatorInfo.h"

CoordinatorInfo::CoordinatorInfo(
        UUIDPtr uuid,
        UUIDPtr cellID,
        SAPInfoPtr meshSAP,
        SAPInfoPtr discoverySAP,
        SAPInfoPtr FTSAP,
        EndpointPtr& cellCoordEndpoint) :
m_uuid(uuid), m_cellID(cellID), m_meshSAP(meshSAP),
m_discoverySAP(discoverySAP), m_FTSAP(FTSAP), m_cellCoordEndpoint(cellCoordEndpoint) {
}

CoordinatorInfo::CoordinatorInfo(const CoordinatorInfo& orig) :
m_uuid(orig.m_uuid), m_cellID(orig.m_cellID), m_meshSAP(orig.m_meshSAP),
m_discoverySAP(orig.m_discoverySAP), m_FTSAP(orig.m_FTSAP),
m_cellCoordEndpoint(orig.m_cellCoordEndpoint) {
}

CoordinatorInfo::~CoordinatorInfo() {
}

UUIDPtr& CoordinatorInfo::getPID() {
    return m_uuid;
}

UUIDPtr& CoordinatorInfo::getCellID() {
    return m_cellID;
}

SAPInfoPtr& CoordinatorInfo::getMeshSap() {
    return m_meshSAP;
}

SAPInfoPtr& CoordinatorInfo::getDiscoverySap() {
    return m_discoverySAP;
}

SAPInfoPtr& CoordinatorInfo::getFTSap() {
    return m_FTSAP;
}

EndpointPtr& CoordinatorInfo::getCellCoordinatorEndpoint() {
    return m_cellCoordEndpoint;
}


