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
 * File:   ServiceDiscoveryProvider.cpp
 * Author: rmartins
 * 
 * Created on August 2, 2010, 5:39 PM
 */

#include "stheno/core/SthenoCore.h"


#include "ServiceDiscoveryProvider.h"
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/discovery/Discovery.h>
#include <stheno/core/p2p/discovery/service_provider/ServiceQuery.h>
#include <stheno/core/p2p/discovery/service_provider/ServiceQueryReply.h>
#include <stheno/core/SthenoCoreInterface.h>

const UInt ServiceDiscoveryProvider::SERVICE_PROVIDER_TYPE = 0x9912AA;

ServiceDiscoveryProvider::ServiceDiscoveryProvider(Discovery* discovery) : m_discovery(discovery) {
    m_providedEvents.push_back(ServiceQuery::SERVICE_QUERY);
}

ServiceDiscoveryProvider::~ServiceDiscoveryProvider() {
}

void ServiceDiscoveryProvider::close() {
}

DiscoveryQueryReply* ServiceDiscoveryProvider::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) throw (DiscoveryException&) {
    if (query->getType() != ServiceQuery::SERVICE_QUERY) {
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    
    ServiceQuery* serviceQ = static_cast<ServiceQuery*> (query);
    try{
        //list<UUIDPtr>* info = m_discovery->getOverlay()->getRuntime()->getInstancesOfService(serviceQ->getSID());
        list<ServiceInstanceInfoPtr>* info = m_discovery->getOverlay()->getRuntime()->getInstancesOfService(serviceQ->getSID());
        if(info == 0){
            return 0;
        }
        ServiceQueryReply* queryReply = new ServiceQueryReply(*info);
        return queryReply;        
    }catch(ServiceException& ex){
        String msg("Unable to execute sync query");
        throw DiscoveryException(msg);
    }
    
    return 0;
}

AsyncDiscoveryQueryReply* ServiceDiscoveryProvider::executeAsyncQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos, ACE_Time_Value* timeout) throw (DiscoveryException&) {
    /*if (query->getType() != ROUTING_QUERY) {
        throw DiscoveryException();
    }
    PeerQuery* peerQ = static_cast<PeerQuery*> (query);
    try {
        PeerInfoPtr* info = getPeerMap().getPeerInfo(peerQ->getUUID());
        PeerQueryReply* peerQR = new PeerQueryReply(*info);
        AsyncDiscoveryQueryReply* asyncQR = new AsyncDiscoveryQueryReply(timeout);
        asyncQR->setFuture(peerQR);
        return asyncQR;

    } catch (PeerException& pEx) {
        //must use remote peers
        return remoteAsyncQuery(query, qos, timeout);
        //throw DiscoveryException();
    }*/
    return 0;
}

list<UInt>& ServiceDiscoveryProvider::getProvidedEvents() {
    return m_providedEvents;
}
