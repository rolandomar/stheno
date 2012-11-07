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
 * File:   P3GeneralProvider.cpp
 * Author: rmartins
 * 
 * Created on October 14, 2010, 12:18 PM
 */

#include "P3GeneralProvider.h"
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQuery.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQueryReply.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQuery.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQueryReply.h>
#include <stheno/core/p2p/discovery/general_provider/FindPeerQuery.h>
#include <stheno/core/p2p/discovery/general_provider/FindPeerQueryReply.h>
#include <stheno/core/p2p/discovery/general_provider/FindServiceInstanceQuery.h>
#include <stheno/core/p2p/discovery/general_provider/FindServiceInstanceQueryReply.h>

//P3 specific queries
#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQuery.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQueryReply.h>



#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>

//const UInt P3GeneralProvider::P3GENERALPROVIDER_TYPE = 0x2138AA;

P3GeneralProvider::P3GeneralProvider(P3Discovery* discovery) : m_discovery(discovery) {
    m_providedEvents.push_back(GetSensorsQuery::GETSENSORS_QUERY);
    m_providedEvents.push_back(LaunchServiceSiteQuery::LAUNCHSERVICESITE_QUERY);
    m_providedEvents.push_back(FindPeerQueryReply::FINDPEER_QUERY_REPLY);
    m_providedEvents.push_back(FindServiceInstanceQuery::FINDSERVICEINSTANCE_QUERY);
    m_providedEvents.push_back(P3FindPeerQuery::P3FINDPEER_QUERY);
    m_providedEvents.push_back(FindPeerQuery::FINDPEER_QUERY);
}

P3GeneralProvider::~P3GeneralProvider() {
}

void P3GeneralProvider::close() {
}

DiscoveryQueryReply* P3GeneralProvider::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) throw (DiscoveryException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider::executeQuery(%d)\n"),query->getType()));
    if (query->getProvider() != GENERALPROVIDER_TYPE) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3GeneralProvider::executeQuery wrong provider\n")));
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    //if (query->getType() != GetSensorsQuery::GETSENSORS_QUERY) {
    if (!isEventProvided(query->getType())) {
        String msg("Unable to execute request");
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider event not provided!!(%d)\n"),query->getType()));
        throw DiscoveryException(msg);
    }
    MeshPtr meshPtr;
    try {
        m_discovery->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider no mesh!(%d)\n"),query->getType()));
        throw DiscoveryException(DiscoveryException::INVALID_OVERLAY);
    }
    P3Mesh* mesh = static_cast<P3Mesh*> (meshPtr.get());
    switch (query->getType()) {
        case GetSensorsQuery::GETSENSORS_QUERY:
        {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)P3GeneralProvider::executeQuery() - 1\n")));
            GetSensorsQuery* query_i = new GetSensorsQuery(query);
            if (mesh->isRootFID()) {
                list<SensorInfoPtr> audioL;
                list<SensorInfoPtr> videoL;
                list<SensorInfoPtr> eventL;
                list<SensorInfoPtr> actuatorL;
                mesh->getAudioSensors(audioL);
                mesh->getVideoSensors(videoL);
                mesh->getEventSensors(eventL);
                mesh->getActuatorSensors(actuatorL);
                GetSensorsQueryReply* reply = new GetSensorsQueryReply(audioL, videoL, eventL, actuatorL);
                return reply;
            }
            return 0;
        }
        case LaunchServiceSiteQuery::LAUNCHSERVICESITE_QUERY:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider::LAUNCHSERVICESITE_QUERY()\n")));
            LaunchServiceSiteQuery* query_i = new LaunchServiceSiteQuery(query);
            if (mesh->isRootFID()) {
                P3PeerInfoPtr peer;
                list<UUIDPtr> filter(query_i->getFilterPeers());
                filter.push_back(query_i->getRequester());
                mesh->getPoL(query_i->getSID(), peer, filter);
                if (peer.null()) {
                    return 0;
                }
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider::LAUNCHSERVICESITE_QUERY() - %s %s\n"),
                        peer->getPID()->toString().c_str(),peer->getFID()->toString().c_str()
                        ));
                LaunchServiceSiteQueryReply* reply =
                        new LaunchServiceSiteQueryReply(
                        (UUIDPtr&) peer->getPID(),
                        (UUIDPtr&) peer->getFID(),
                        peer->getMeshSAP(), peer->getFTSAP());
                return reply;
            }
            return 0;
        }

        case FindPeerQuery::FINDPEER_QUERY:
        {
            FindPeerQuery* query_i = new FindPeerQuery(query);
            P3PeerInfoPtr pi;
            if (mesh->getPeer(query_i->getUUID(), pi) == 0) {
                PeerInfo *a = new PeerInfo(pi->getType(), pi->getPID(), pi->getFID(), pi->getMeshSAP(), pi->getDiscoverySAP(),
                        pi->getFTSAP());
                PeerInfoPtr piPTr(a);
                //PeerInfoPtr& piPTr = (PeerInfoPtr&) pi;
                printf("===============================%p\n",piPTr.get());
                printf("===============================%s\n",piPTr->getPID()->toString().c_str());
                FindPeerQueryReply* reply = new FindPeerQueryReply(piPTr);
                return reply;
            }else{
                printf("====================== Not found! %s\n",query_i->getUUID()->toString().c_str());
            }
            return 0;
        }
        case P3FindPeerQuery::P3FINDPEER_QUERY:
        {
            P3FindPeerQuery* query_i = new P3FindPeerQuery(query);
            P3PeerInfoPtr pi;
            if (query_i->getTreeLevel() == INT_MAX) {
                if (mesh->getRandomPeer(pi) == 0) {
                    //PeerInfoPtr& piPTr = (PeerInfoPtr&) pi;
                    PeerInfo *a = new PeerInfo(pi->getType(), pi->getPID(), pi->getFID(), pi->getMeshSAP(), pi->getDiscoverySAP(),
                            pi->getFTSAP());
                    PeerInfoPtr piPTr(a);
                    P3FindPeerQueryReply* reply = new P3FindPeerQueryReply(piPTr);
                    return reply;
                }
            } else {
                if (mesh->getPeerByLevel(query_i->getTreeLevel(), pi) == 0) {
                    //PeerInfoPtr& piPTr = (PeerInfoPtr&) pi;
                    PeerInfo *a = new PeerInfo(pi->getType(), pi->getPID(), pi->getFID(), pi->getMeshSAP(), pi->getDiscoverySAP(),
                            pi->getFTSAP());
                    PeerInfoPtr piPTr(a);
                    P3FindPeerQueryReply* reply = new P3FindPeerQueryReply(piPTr);
                    return reply;
                }
            }
            return 0;
        }


        case FindServiceInstanceQuery::FINDSERVICEINSTANCE_QUERY:
        {
            //P3Mesh* mesh = static_cast<P3Mesh*> (m_discovery->getOverlay()->getMesh().get());
            FindServiceInstanceQuery* query_i = new FindServiceInstanceQuery(query);
            //FindServiceInstanceQuery* query_i = static_cast<FindServiceInstanceQuery*> (query);
            SAPInfoPtr pi;
            if (mesh->getPeerInfoByIID(query_i->getSID(), query_i->getIID(), pi) == 0) {
                printf("\n\n\n\nFound instance\n\n\n\n");
                UUIDPtr runtimeUUID;
                mesh->getUUID(runtimeUUID);
                UUIDPtr fid;
                mesh->getFID(fid);
                FindServiceInstanceQueryReply* reply = new FindServiceInstanceQueryReply(runtimeUUID, fid, pi);
                /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider::executeQuery() - FindServiceInstanceQuery::FINDSERVICEINSTANCE_QUERY: %@\n"), 
                        pi.get()));*/
                return reply;
            }
            printf("\n\n\n\nNOt found Found instance %s\n\n\n\n",query_i->getIID()->toString().c_str());
            /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3GeneralProvider::executeQuery() - FindServiceInstanceQuery::FINDSERVICEINSTANCE_QUERY: not found!\n")
            ));*/
            return 0;
        }
    }
    return 0;
}

AsyncDiscoveryQueryReply* P3GeneralProvider::executeAsyncQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos/*, ACE_Time_Value* timeout*/) throw (DiscoveryException&) {
    if (query->getProvider() != GENERALPROVIDER_TYPE) {
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    if (query->getType() != GetSensorsQuery::GETSENSORS_QUERY) {
        String msg("Unable to execute request");
        throw DiscoveryException(msg);
    }
    //P3Mesh* mesh = static_cast<P3Mesh*> (m_discovery->getOverlay()->getMesh().get());
    MeshPtr meshPtr;
    try {
        m_discovery->getOverlay()->getMesh(meshPtr);
    } catch (OverlayException& ex) {
        throw DiscoveryException(DiscoveryException::INVALID_OVERLAY);
    }
    P3Mesh* mesh = static_cast<P3Mesh*> (meshPtr.get());
    AsyncDiscoveryQueryReply* asyncQR = new AsyncDiscoveryQueryReply(0/*timeout*/);
    switch (query->getType()) {
        case GetSensorsQuery::GETSENSORS_QUERY:
        {
            /*list<P3PeerInfoPtr> sensorList;
            //P3Mesh* mesh = static_cast<P3Mesh*> (m_meshService.get());            
            mesh->getSensorList(sensorList);
            list<PeerInfoPtr> listPeerInfo;
            list<P3PeerInfoPtr>::iterator iter = sensorList.begin();
            while (iter != sensorList.end()) {
                PeerInfoPtr iPtr(new PeerInfo(*(*iter).get()));
                listPeerInfo.push_back(iPtr);
                iter++;
            }
            if (sensorList.size() != 0) {
                GetSensorsQueryReply* reply = new GetSensorsQueryReply(listPeerInfo);
                asyncQR->setFuture(reply);
            } else {
                asyncQR->cancel();
            }
            return asyncQR;*/
            return 0;
        }
        case LaunchServiceSiteQuery::LAUNCHSERVICESITE_QUERY:
        {
            LaunchServiceSiteQuery* query_i = static_cast<LaunchServiceSiteQuery*> (query);
            if (mesh->isRootFID()) {
                P3PeerInfoPtr peer;
                list<UUIDPtr> filter;
                filter.push_back(query_i->getRequester());
                mesh->getPoL(query_i->getSID(), peer, filter);
                LaunchServiceSiteQueryReply* reply =
                        new LaunchServiceSiteQueryReply((UUIDPtr&) peer->getPID(), (UUIDPtr&) peer->getFID(), peer->getMeshSAP(), peer->getFTSAP());
                asyncQR->setFuture(reply);
                return asyncQR;
            }
        }
        case FindPeerQuery::FINDPEER_QUERY:
        {
            return 0;
        }
        case FindServiceInstanceQuery::FINDSERVICEINSTANCE_QUERY:
        {
            return 0;
        }
    }
    return 0;
}

list<UInt>& P3GeneralProvider::getProvidedEvents() {
    return m_providedEvents;
}

void P3GeneralProvider::cancelAsyncQuery(AsyncDiscoveryQueryReply* token) throw (DiscoveryException&) {
    if (token == 0) {
        String msg("Null Token!");
        throw DiscoveryException(msg);
    }
    token->cancel();
}

UInt P3GeneralProvider::getProviderID() {
    return GENERALPROVIDER_TYPE;
}