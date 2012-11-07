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
 * File:   PeerProvider.cpp
 * Author: rmartins
 * 
 * Created on July 26, 2010, 6:13 PM
 */

#include "PeerProvider.h"
#include <stheno/core/p2p/discovery/peer_provider/PeerQuery.h>
#include <stheno/core/p2p/discovery/peer_provider/PeerQueryReply.h>

const UInt PeerProvider::PEER_PROVIDER = 0x45452A;

PeerProvider::PeerProvider(MeshPtr& meshService) : m_meshService(meshService) {
    m_providedEvents.push_back(PeerQuery::ROUTING_QUERY);
}

PeerProvider::~PeerProvider() {
}

void PeerProvider::close() {
}

DiscoveryQueryReply* PeerProvider::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) throw (DiscoveryException&) {
    if (query->getType() != PeerQuery::ROUTING_QUERY) {
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    PeerQuery* peerQ = static_cast<PeerQuery*> (query);
    try {
        //PeerInfoPtr* info = getPeerMap().getPeerInfo(peerQ->getUUID());
        PeerInfoPtr info;
        getPeerMap()->getPeerInfo(peerQ->getUUID(), info);
        PeerQueryReply* peerQR = new PeerQueryReply(info);
        return peerQR;

    } catch (PeerException& pEx) {
        //must use remote peers
        //return removeQuery(query, qos);
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    return 0;
}

AsyncDiscoveryQueryReply* PeerProvider::executeAsyncQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos, ACE_Time_Value* timeout) throw (DiscoveryException&) {
    if (query->getType() != PeerQuery::ROUTING_QUERY) {
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    PeerQuery* peerQ = static_cast<PeerQuery*> (query);
    try {
        PeerInfoPtr info;
        getPeerMap()->getPeerInfo(peerQ->getUUID(), info);
        //PeerInfoPtr* info = getPeerMap().getPeerInfo(peerQ->getUUID());
        PeerQueryReply* peerQR = new PeerQueryReply(info);
        AsyncDiscoveryQueryReply* asyncQR = new AsyncDiscoveryQueryReply(timeout);
        asyncQR->setFuture(peerQR);
        return asyncQR;

    } catch (PeerException& pEx) {
        //must use remote peers
        //return remoteAsyncQuery(query, qos, timeout);
        String msg("Unable to execute async query");
        throw DiscoveryException(msg);
    }
    return 0;
}

list<UInt>& PeerProvider::getProvidedEvents() {
    return m_providedEvents;
}

void PeerProvider::cancelAsyncQuery(AsyncDiscoveryQueryReply* token) throw (DiscoveryException&) {
    if (token == 0) {
        String msg("Null Token!");
        throw DiscoveryException(msg);
    }
    token->cancel();
}

UInt PeerProvider::getProviderID() {
    return PEER_PROVIDER;
}

PeerMapPtr& PeerProvider::getPeerMap() {
    return m_meshService->getPeerMap();
}