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
 * File:   LeafPeer.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 30, 2010, 12:27 PM
 */

#include "LeafPeer.h"

#include <stheno/core/p2p/p3/common/P3PeerInfo.h>
#include <stheno/core/p2p/p3/leaf/mesh/P3LeafMesh.h>
#include <stheno/core/p2p/p3/leaf/discovery/LeafDiscovery.h>
#include <euryale/common/Backtrace.h>
#include <stheno/core/p2p/p3/leaf/mesh/SensorInfoUpdate.h>
#include <stheno/core/p2p/p3/leaf/stream/StreamServer.h>
#include <stheno/core/p2p/p3/leaf/actuator/ActuatorServer.h>

LeafPeer::LeafPeer() : P3(),
m_type(P3PeerInfo::SENSOR) {
}

LeafPeer::LeafPeer(SthenoCoreInterface* runtime) :
P3(runtime), m_meshService(0), m_type(P3PeerInfo::SENSOR) {
}

LeafPeer::LeafPeer(int type) :
P3(), m_type(type) {
}

LeafPeer::LeafPeer(int type, SthenoCoreInterface* runtime) :
P3(runtime), m_meshService(0), m_type(type) {
}

LeafPeer::~LeafPeer() {
}

void LeafPeer::getDiscovery(DiscoveryPtr& discoveryPtr) throw (OverlayException&) {
    discoveryPtr = m_discoveryPtr;
}

void LeafPeer::getMesh(MeshPtr& meshPtr) throw (OverlayException&) {
    meshPtr = m_meshPtr;
}

void LeafPeer::getFaultTolerance(FaultTolerancePtr& ftPtr) throw (OverlayException&) {
    throw OverlayException(OverlayException::INVALID_OVERLAY);
}

void LeafPeer::open() throw (OverlayException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)LeafPeer::open(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toStarting();
    open_i();
}

void LeafPeer::open_i() throw (OverlayException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafPeer::open_i - status(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (isStarting() /*|| isResuming()*/) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafPeer::open_i - starting(%d)\n"), m_status));
        m_meshService = new P3LeafMesh(this);
        m_discoveryService = new LeafDiscovery(this);
        try {
            ServiceParamsPtr nullParams;
            //TODO: change this
            //QoSManagerInterface* discoveryQoSManager = getQoSManager();
            QoSManagerInterface* discoveryQoSManager = 0;
            if (getQoSManager() != 0) {
                discoveryQoSManager = getQoSManager()->createSubDomainQoSManager("discovery", 5, CPUReservation::DEFAULT_PERIOD);
            }
            m_discoveryService->open(nullParams, ServiceAbstract::FT_NOT_SETTED, discoveryQoSManager);
            m_discoveryPtr.reset(m_discoveryService);

            //TODO: change this
            //QoSManagerInterface* meshQoSManager = getQoSManager();
            QoSManagerInterface* meshQoSManager = 0;
            if (getQoSManager() != 0) {
                meshQoSManager = getQoSManager()->createSubDomainQoSManager("mesh", 5, CPUReservation::DEFAULT_PERIOD);
            }
            m_meshService->open(nullParams, ServiceAbstract::FT_NOT_SETTED, meshQoSManager);
            m_meshPtr.reset(m_meshService);

            if (m_type == P3PeerInfo::SENSOR) {
                QoSManagerInterface* streamQoSManager = 0;
                if (getQoSManager() != 0) {
                    streamQoSManager = getQoSManager()->createSubDomainQoSManager("stream", 5, CPUReservation::DEFAULT_PERIOD);
                }
                m_streamServer = new StreamServer(StreamServer::AUDIO_SENSOR);
                m_streamServer->open(streamQoSManager);
                //Bootstrap sensor services
                EndpointPtr ePtr(m_streamServer->getEndpoint());
                UUIDPtr runtimeUUID;
                this->getUUID(runtimeUUID);
                SensorInfoUpdate* info = new SensorInfoUpdate(SensorInfoUpdate::SENSOR_UPDATE_AUDIO_OP,
                        runtimeUUID, ePtr);
                InfoUpdatePtr infoPtr(info);
                bool st = m_meshService->updateInfo(infoPtr);
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafPeer::open() - updated stream info(%d)\n"), st));
                //open actuator
                QoSManagerInterface* actuatorQoSManager = 0;
                if (getQoSManager() != 0) {
                    actuatorQoSManager = getQoSManager()->createSubDomainQoSManager("actuator", 5, CPUReservation::DEFAULT_PERIOD);
                }
                m_actuatorServer = new ActuatorServer();
                m_actuatorServer->open(actuatorQoSManager);
                EndpointPtr actuatorEndpointPtr(m_actuatorServer->getEndpoint());
                SensorInfoUpdate* actuatorInfo = new SensorInfoUpdate(SensorInfoUpdate::SENSOR_UPDATE_ACTUATOR_OP,
                        runtimeUUID, actuatorEndpointPtr);
                InfoUpdatePtr actuatorInfoPtr(actuatorInfo);
                st = m_meshService->updateInfo(actuatorInfoPtr);
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafPeer::open() - updated actuator info(%d)\n"), st));
            }



        } catch (ServiceException& ex) {
            //Backtrace::backstrace();
            throw OverlayException(OverlayException::BOOTSTRAP_FAILED);
        }
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: LeafPeer::open() - error\n")));
        throw OverlayException(OverlayException::BOOTSTRAP_FAILED);
    }
}

void LeafPeer::close_i() throw (OverlayException&) {
    // P3::close_i();
    /*if (m_meshService != 0) {
        m_meshService->close();
    }*/

    if (m_meshService != 0) {
        m_meshService->close();
        m_meshService = 0;
    }

    if (m_discoveryService != 0) {
        m_discoveryService->close();
        m_discoveryService = 0;
    }

}

LeafDiscovery* LeafPeer::getP3Discovery() {
    return m_discoveryService;
}

P3LeafMesh* LeafPeer::getP3MeshService() {
    return m_meshService;
}

int LeafPeer::getType() {
    return m_type;
}

void LeafPeer::getPID(UUIDPtr& uuid) {
    getUUID(uuid);
}
