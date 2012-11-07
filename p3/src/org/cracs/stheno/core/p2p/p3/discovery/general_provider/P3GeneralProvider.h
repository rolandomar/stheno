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
 * File:   P3GeneralProvider.h
 * Author: rmartins
 *
 * Created on October 14, 2010, 12:18 PM
 */

#ifndef P3GENERALPROVIDER_H
#define	P3GENERALPROVIDER_H

#include <stheno/core/p2p/discovery/DiscoveryProvider.h>
#include <stheno/core/p2p/common/PeerMap.h>
#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/p2p/discovery/general_provider/GeneralProvider.h>
//abstract PeerProvider, remote access must be provided
/***/
class P3Discovery;
class P3GeneralProvider: public DiscoveryProvider {
public:
    
    //static const UInt P3GENERALPROVIDER_TYPE;
            
    P3GeneralProvider(P3Discovery* discovery);    
    virtual ~P3GeneralProvider();
    
    
    virtual void close();
    
    virtual DiscoveryQueryReply* executeQuery(
        DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) throw (DiscoveryException&);
    
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(
        DiscoveryQuery* query,DiscoveryQoS* qos = 0/*, 
        ACE_Time_Value* timeout = 0*/) throw (DiscoveryException&);   
    
    virtual void cancelAsyncQuery(AsyncDiscoveryQueryReply* token) throw (DiscoveryException&);
    //caller must free list
    virtual list<UInt>& getProvidedEvents();
        
    
    UInt getProviderID();
    
protected:    
    P3Discovery* m_discovery;
    list<UInt> m_providedEvents;
};


#endif	/* P3GENERALPROVIDER_H */

