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
 * File:   Membership.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 12, 2010, 11:29 AM
 */

#include "Mesh.h"
#include <stheno/core/p2p/ft/FaultTolerance.h>
#include <stheno/service/UserService.h>
#include <stheno/core/p2p/Overlay.h>

Mesh::Mesh(Overlay* overlay) : P2PService(id(), overlay) {
}

Mesh::~Mesh() {
}

void Mesh::createLocalService(UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid) throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

void Mesh::closeLocalService(UUIDPtr& sid) throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

UserService* Mesh::createServiceInstance(UUIDPtr& sid) throw (ServiceException&) {
    //return 0;
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

void Mesh::getUUID(UUIDPtr& uuid) {
    getOverlay()->getUUID(uuid);
}

void Mesh::addFederationListener(MeshListener* l, UUIDPtr& fid) throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

void Mesh::addPeerListener(MeshListener* l, UUIDPtr& fid, UUIDPtr& peerID) throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

const char* Mesh::id() {
    return "1CD4CDE9E626B2050046D391A2DF3922";
}

ServiceInfoPtr& Mesh::getServiceInfo(UUIDPtr& serviceUUID) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

int Mesh::getPeerInfoByIID(UUIDPtr& sid, UUIDPtr& iid, PeerInfoPtr& pi) {
    return -1;
}