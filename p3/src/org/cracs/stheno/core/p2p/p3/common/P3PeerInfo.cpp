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
 * File:   TreePeerInfo.cpp
 * Author: rmartins
 * 
 * Created on August 18, 2010, 5:29 PM
 */

#include "P3PeerInfo.h"

P3PeerInfo::P3PeerInfo(InputStream& inputStream) : PeerInfo(), m_cellEndpoint(0) {
    this->deserialize(inputStream);
}

P3PeerInfo::P3PeerInfo(const UUIDPtr& pid,
        const UUIDPtr& fid,
        const EndpointPtr& cellEndpoint,
        const SAPInfoPtr& meshSAP,
        const SAPInfoPtr& discoverySAP,
        const SAPInfoPtr& ftSAP)
: PeerInfo(SUPERPEER, pid, fid,meshSAP,discoverySAP,ftSAP), m_cellEndpoint(cellEndpoint){
}

P3PeerInfo::P3PeerInfo(UInt type,
        const UUIDPtr& pid,
        const EndpointPtr& cellEndpoint,
        const SAPInfoPtr& meshSAP,
        const SAPInfoPtr& discoverySAP,
        const SAPInfoPtr& ftSAP)
: PeerInfo(type, pid, UUID::INVALID_ID,meshSAP,discoverySAP,ftSAP), m_cellEndpoint(cellEndpoint){
}

P3PeerInfo::P3PeerInfo(
        UInt type,
        const UUIDPtr& pid,
        const UUIDPtr& fid,
        const EndpointPtr& cellEndpoint,
        const SAPInfoPtr& meshSAP,
        const SAPInfoPtr& discoverySAP,
        const SAPInfoPtr& ftSAP)
: PeerInfo(type, pid, fid,meshSAP,discoverySAP,ftSAP), m_cellEndpoint(cellEndpoint){
}

P3PeerInfo::~P3PeerInfo() {
}

bool P3PeerInfo::isSensor() {
    return (m_type == SENSOR);
}

bool P3PeerInfo::isSuperpeer() {
    return (m_type == SUPERPEER || m_type == SUPERPEER_COORD);
}

bool P3PeerInfo::isCoordinator() {
    return (m_type == SUPERPEER_COORD);
}

bool P3PeerInfo::isMonitor() {
    return (m_type == MONITOR);
}

void P3PeerInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    PeerInfo::serialize(outputStream);
    if (!m_cellEndpoint.null()) {
        outputStream.write_boolean(true);
        m_cellEndpoint->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }   
}

void P3PeerInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    PeerInfo::deserialize(inputStream);
    Boolean flag = false;
    inputStream.read_boolean(flag);
    if (flag) {
        Endpoint* e = new Endpoint(inputStream);
        m_cellEndpoint.reset(e);
    } else {
        m_cellEndpoint.reset(0);
    }   
}

EndpointPtr& P3PeerInfo::getCellEndpoint() {
    return m_cellEndpoint;
}
