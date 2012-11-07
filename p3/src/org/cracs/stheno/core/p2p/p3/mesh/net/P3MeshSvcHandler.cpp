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
 * File:   P3MeshSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on September 30, 2010, 6:25 PM
 */

#include "P3MeshSvcHandler.h"
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServiceReplyPacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServiceReplyPacket.h>

P3MeshSvcHandler::~P3MeshSvcHandler() {
    /*if (m_requests != 0) {
        delete m_requests;
        m_requests = 0;
    }*/
    close();
}

int P3MeshSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    switch (packet->getPacketHeader()->getType()) {

        case P3MeshCreateServicePacket::P3MESH_CREATESERVICE_PACKET:
        {
            P3MeshCreateServicePacket* packetImp = static_cast<P3MeshCreateServicePacket*> (packet);
            return m_mesh->handleServiceCreate(this, packetImp);
        }

        case P3MeshRemoveServicePacket::P3MESH_REMOVESERVICE_PACKET:
        {
            P3MeshRemoveServicePacket* packetImp = static_cast<P3MeshRemoveServicePacket*> (packet);
            return m_mesh->handleServiceRemove(this, packetImp);
        }
        default:
        {
            delete packet;
            return -1;
        }
    }
    return -1;
    //m_discovery->handlePacket(this,packet);
}