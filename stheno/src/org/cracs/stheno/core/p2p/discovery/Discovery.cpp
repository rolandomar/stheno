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
 * File:   Discovery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 29, 2010, 11:54 AM
 */

#include "Discovery.h"
#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>

#include <stheno/core/p2p/discovery/DiscoveryProvider.h>
#include <euryale/common/ListHelper.h>

Discovery::Discovery(Overlay* overlay) :
P2PService(id(), overlay) {
}

Discovery::~Discovery() {
    //ListHelper<DiscoveryProviderInfo*>::clearList(m_eventProvider);    
    ACE_Hash_Map_Manager<UInt, DiscoveryProviderInfo*,
            ACE_SYNCH_RW_MUTEX>::ITERATOR iter = m_eventProvider.begin();
    while (iter != m_eventProvider.end()) {
        //delete iter->item();        
        iter++;
    }
    m_eventProvider.unbind_all();

}

bool Discovery::addDiscoveryProvider(DiscoveryProvider* provider) throw (ServiceException&) {
    UInt providerID = provider->getProviderID();
    DiscoveryProviderInfo* oldValue = findProvider(providerID);
    if (oldValue == 0) {
        DiscoveryProviderInfo* info =
                new DiscoveryProviderInfo(provider, providerID);
        m_eventProvider.bind(providerID, info);
    } else {
        removeDiscoveryProvider(provider);
        return false;
    }

    return true;
}

bool Discovery::removeDiscoveryProvider(DiscoveryProvider* provider) throw (ServiceException&) {    
    m_eventProvider.unbind(provider->getProviderID());    
    return true;
}

DiscoveryProviderInfo* Discovery::findProvider(UInt providerID) {
    DiscoveryProviderInfo* info;
    if (m_eventProvider.find(providerID, info) == 0) {
        return info;
    }
    return 0;

}