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
 * File:   PeerProvider.h
 * Author: rmartins
 *
 * Created on July 26, 2010, 6:13 PM
 */

#ifndef PEERPROVIDER_H
#define	PEERPROVIDER_H

#include <stheno/core/p2p/discovery/DiscoveryProvider.h>
#include <stheno/core/p2p/common/PeerMap.h>
#include <stheno/core/p2p/mesh/Mesh.h>

class PeerProvider: public DiscoveryProvider {
public:  
    static const UInt PEER_PROVIDER;
    
    PeerProvider(MeshPtr& meshService);    
    virtual ~PeerProvider();
      
    virtual void close();
    
    virtual DiscoveryQueryReply* executeQuery(
        DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) throw (DiscoveryException&);
    
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(
        DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0,
        ACE_Time_Value* timeout = 0) throw (DiscoveryException&);        
    
    virtual void cancelAsyncQuery(AsyncDiscoveryQueryReply* token) throw (DiscoveryException&);
    virtual UInt getProviderID();

    //caller must free list
    virtual list<UInt>& getProvidedEvents();        
protected:    
    list<UInt> m_providedEvents;
    MeshPtr m_meshService;
    
    PeerMapPtr& getPeerMap();    
};

#endif	/* PEERPROVIDER_H */

