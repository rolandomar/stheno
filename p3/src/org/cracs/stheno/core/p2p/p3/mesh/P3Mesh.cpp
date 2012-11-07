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
 * File:   P3Mesh.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 24, 2010, 4:00 PM
 */

#include "P3Mesh.h"
#include "stheno/common/TraceRuntime.h"

#include <stheno/core/p2p/p3/superpeer/cell/discovery/CellDiscovery.h>
#include <stheno/core/p2p/p3/P3.h>
#include <stheno/core/p2p/p3/superpeer/SuperPeer.h>

#include <stheno/core/p2p/p3/mesh/net/P3MeshSvcHandler.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServiceReplyPacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServiceReplyPacket.h>

#include <stheno/core/p2p/p3/mesh/net/P3MeshClientHandler.h>
#include <stheno/core/SthenoCoreInterface.h>
#include <stheno/core/p2p/p3/mesh/MeshUpdateInfo.h>
#include <stheno/core/p2p/p3/mesh/AddServiceUpdateInfo.h>
#include <stheno/core/p2p/p3/mesh/ChangegIIDServiceUpdateInfo.h>

#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

P3Mesh::P3Mesh(P3* overlay) : Mesh(overlay), m_sap(0), m_cell(0), m_peerMap(new PeerMap()) {


}

P3Mesh::~P3Mesh() {
}

bool P3Mesh::addPeer(P3PeerInfoPtr& peer) {
    //conversion to interface type
    PeerInfoPtr newPeerPtr(new P3PeerInfo(*(peer.get())));
    return m_peerMap->addPeer(newPeerPtr);
}

bool P3Mesh::isRootFID() {
    if (m_cell == 0) {
        return false;
    }
    return m_cell->isRoot();
}

void P3Mesh::getParentUUID(UUIDPtr& parentUUID) throw (ServiceException&) {
    m_cell->getParentUUID(parentUUID);
}

void P3Mesh::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i(%d)\n"), m_status));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (isStarting() /*|| isResuming()*/) {
        if (m_sap == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP...\n"), m_status));
            //QoS Domain
            /*String domainPath = "runtime_" + this->getUUID()->toString();
            domainPath += "/p2p/mesh";
            CPUGroupQoS qos("/HRT", domainPath, 10000, QoSManagerInterface::DEFAULT_PERIOD, 1024);
            this->getOverlay()->getRuntime()->getQoSManager()->createGroupCPUReserve(domainPath, qos);*/


            list<QoSEndpoint> qosList;
            //QoSEndpoint hrt(QoS::HRT, 50);
            //QoSEndpoint srt(QoS::SRT, 25);
            QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
            QoSEndpoint srt(QoS::SRT, CPUQoS::MED_RT_PRIO);
            QoSEndpoint be(QoS::BE, CPUQoS::BE_PRIO);
            qosList.push_back(hrt);
            qosList.push_back(srt);
            qosList.push_back(be);
            String itf;
            if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i(%d) creating SAP... exception!\n"), m_status));
                throw ServiceException(ServiceException::INVALID_ARGUMENT);
            }
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 2....\n"), m_status));
            m_sap = new P3MeshSAP(this);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) creating SAP 3....\n"), m_status));
            m_sap->open(itf, qosList);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i(%d) created SAP\n"), m_status));
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3Mesh::open_i starting(%d)\n"), m_status));
        //Task::activate();
        int maxBindTries = 1;
        ACE_Time_Value start = ACE_OS::gettimeofday();
        for (int i = 0; i < maxBindTries; i++) {
            try {
                bind();
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open_i - end, status(%d)\n"), m_status));
                ACE_Time_Value end = ACE_OS::gettimeofday();
                end -= start;                
                TraceRuntimeSingleton::instance()->logMembershipTime(end);
                return;
            } catch (ServiceException& bindEx) {
                //discard bindEx
            }
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open(): bind failed\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::open(): error\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
}

void P3Mesh::close_i() throw (ServiceException&) {
    if (m_cell != 0) {
        m_cell->close();
        m_cell = 0;
    }
}

void P3Mesh::bind() throw (ServiceException&) {
    if (m_cell != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeer::bind():open ex=ServiceException::REGISTRATION_ERROR\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)SuperPeer::bind():open\n")));
    //TODO: change this    
    QoSManagerInterface* qosManager = this->getQoSManager();
    UUIDPtr runtimeUUID;
    this->getUUID(runtimeUUID);
    CellDiscovery* discovery = new CellDiscovery(runtimeUUID, qosManager);
    discovery->open();
    CellReply* info = discovery->requestCell(CellID::INVALID_CELL_ID_UUIDPTR);
    m_cell = new Cell(this, qosManager);
    m_cell->open(info, discovery);   
}

int P3Mesh::getPeer(UUIDPtr& uuid, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return m_cell->getPeer(uuid, info);
}

int P3Mesh::getPeerByLevel(int treeLevel, P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return m_cell->getPeerByLevel(treeLevel, info);
}

int P3Mesh::getRandomPeer(P3PeerInfoPtr& info) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    return m_cell->getRandomPeer(info);
}

void P3Mesh::getNeighbors(list<PeerLocation>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_cell->getNeighbors(l);
}

void P3Mesh::getDiscoverySAPInfo(UUIDPtr& uuid, SAPInfoPtr& si) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_cell->getDiscoverySAPInfo(uuid, si);
}

void P3Mesh::updateState(InfoUpdate* update) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_cell->updateState(update);
}

void P3Mesh::getSAP(SAPInfoPtr& sapInfo) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    sapInfo = m_sap->getSAPInfo();

}

int P3Mesh::handleServiceCreate(P3MeshSvcHandler* channel, P3MeshCreateServicePacket* packet) {
    UUIDPtr iid;
    //ServiceParams* params = packet->getServiceParams()->duplicate();
    ServiceParamsPtr params(packet->getServiceParams());
    P3MeshCreateServiceReplyPacket* replyPacket = 0;
    try {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::createRemoteService - creating service...\n")));
        //UUIDPtr sid(params->getSID());
        this->getOverlay()->getRuntime()->createLocalService(params->getSID(), params, iid);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::createRemoteService - created service!\n")));

        replyPacket =
                new P3MeshCreateServiceReplyPacket(packet->getPacketHeader()->getPacketID(), iid);
    } catch (ServiceException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::createRemoteService - creating service - EXception!.\n")));
        String errorMsg("failed creation");
        replyPacket =
                new P3MeshCreateServiceReplyPacket(packet->getPacketHeader()->getPacketID(), new MeshException(errorMsg));
    }

    return channel->sendPacket((SthenoStreamPacket*&) replyPacket);
}

int P3Mesh::handleServiceRemove(P3MeshSvcHandler* channel, P3MeshRemoveServicePacket* packet) {
    UUIDPtr iid;
    //TODO: process remove service
    P3MeshRemoveServiceReplyPacket* replyPacket =
            new P3MeshRemoveServiceReplyPacket(packet->getPacketHeader()->getPacketID());
    return channel->sendPacket((SthenoStreamPacket*&) replyPacket);
}

SuperPeer* P3Mesh::getOverlay_i() {
    return static_cast<SuperPeer*> (m_overlay);
}

void P3Mesh::getFID(UUIDPtr& fid) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_cell->getFID(fid);
}

void P3Mesh::getSensorList(list<P3PeerInfoPtr>& l) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_cell->getSensors(l);
}

void P3Mesh::getPoL(const UUIDPtr& sid, P3PeerInfoPtr& l, list<UUIDPtr>& filter) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    //list<UUIDPtr> filter;
    //filter.push_back(this->getUUID());
    m_cell->getPoL(l, filter);
}

int P3Mesh::getPeerInfoByIID(UUIDPtr& sid, UUIDPtr& iid, SAPInfoPtr& pi) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    int ret = m_cell->getPeerByIID(sid, iid, pi);
    if (ret == 0) {

    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::getPeerInfoByIID - didnt find (sid,iid) (%s,%s)\n"),
                sid->toString().c_str(), iid->toString().c_str()                
                ));
        ServiceInstanceInfoPtr info;
        try {
            this->getOverlay()->getRuntime()->getInstancesOfService(sid, iid, info);
        } catch (ServiceException& ex) {
            return -1;
        }

        if (info.null()) {
            return -1;
        }
        pi = info->getSAPInfo();
        return 0;
    }
    return ret;
}

void P3Mesh::getUUID(UUIDPtr& uuid) {
    Mesh::getUUID(uuid);
}

void P3Mesh::createRemoteService(const SAPInfo* hint, const UUIDPtr& uuid, const UUIDPtr& sid, ServiceParamsPtr& params, UUIDPtr& iid) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    P3PeerInfoPtr peer;
    int ret = m_cell->getPeer(uuid, peer);
    if (ret == -1) {
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    //P3MeshClientHandler
    Endpoint endpoint;
    ACE_Connector<P3MeshClientHandler, ACE_SOCK_Connector> connector;
    peer->getMeshSAP()->getFirstEndpoint(endpoint);
    QoSEndpoint qosE = *(endpoint.getQoS());
    
    UUIDPtr runtimeUUID;
    m_cell->getPID(runtimeUUID);
    UUIDPtr fid;
    m_cell->getFID(fid);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::createRemoteService - send to coordinator/parent FID=%@\n"),
            fid.get()));

    //UUIDPtr runtimeUUID;
    //this->getUUID(runtimeUUID);
    P3MeshClientHandler* clientHandler = new P3MeshClientHandler(
            runtimeUUID,
            fid,
            qosE,
            false, false, 0, 0, 0, 0);

    if (connector.connect(clientHandler, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: P3Mesh::createRemoteService - connect failed:")));
        clientHandler->close();
        clientHandler = 0;
        delete clientHandler;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::createRemoteService - Connect OK!\n")));
    }

    ret = clientHandler->createService(params, iid);
    clientHandler->close();
    delete clientHandler;
    if (ret == -1) {
        throw ServiceException(ServiceException::INSUFFICIENT_RESOURCES);
    }
}

void P3Mesh::closeRemoteService(UUIDPtr& uuid, UUIDPtr& sid) throw (ServiceException&) {

}

QoSResources* P3Mesh::calculateQoSResources(ServiceParamsPtr& params) {
    return 0;
}

const char* P3Mesh::getName() {
    return "P3Mesh";
}

PeerMapPtr& P3Mesh::getPeerMap() {
    return m_peerMap;
}

void P3Mesh::getAudioSensors(list<SensorInfoPtr>& audioL) {
    m_cell->getAudioSensors(audioL);
}

void P3Mesh::getVideoSensors(list<SensorInfoPtr>& videoL) {
    m_cell->getVideoSensors(videoL);
}

void P3Mesh::getEventSensors(list<SensorInfoPtr>& eventL) {
    m_cell->getEventSensors(eventL);
}

void P3Mesh::getActuatorSensors(list<SensorInfoPtr>& actuatorL) {
    m_cell->getActuatorSensors(actuatorL);
}

void P3Mesh::onServiceCreation(ServiceInstanceInfoPtr& sii) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::onServiceCreation()\n")));
    UUIDPtr runtimeUUID;
    getUUID(runtimeUUID);
    UUIDPtr fid;
    getFID(fid);
    AddServiceUpdateInfo* addInfo =
            new AddServiceUpdateInfo(runtimeUUID,fid, sii);
    this->updateState(addInfo);
}

void P3Mesh::onServiceUpdateIID(UUIDPtr& sid, UUIDPtr& oldIid, UUIDPtr& newIid) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Mesh::onServiceUpdateIID()\n")));
    UUIDPtr runtimeUUID;
    getUUID(runtimeUUID);
    UUIDPtr fid;
    getFID(fid);
    ChangegIIDServiceUpdateInfo* updateInfo =
            new ChangegIIDServiceUpdateInfo(runtimeUUID,fid, sid, oldIid, newIid);
    this->updateState(updateInfo);
}