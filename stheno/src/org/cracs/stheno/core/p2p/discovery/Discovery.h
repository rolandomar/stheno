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
 * File:   Discovery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 29, 2010, 11:54 AM
 */

#ifndef DISCOVERY_H
#define	DISCOVERY_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/Types.h>


#include <stheno/core/p2p/P2PService.h>

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/discovery/AsyncDiscoveryQueryReply.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>
#include <stheno/core/p2p/discovery/exception/DiscoveryException.h>
#include <stheno/core/p2p/discovery/DiscoveryProviderInfo.h>

class Overlay;
class Discovery: public P2PService {
public:
    
    virtual ~Discovery();

    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) = 0;
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0) = 0;    
    
    virtual const char* id(){
        return "AFC8425E5454D05E991866840F8A65CD";
    }
    
    virtual bool addDiscoveryProvider(DiscoveryProvider* provider) throw(ServiceException&);
    virtual bool removeDiscoveryProvider(DiscoveryProvider* provider) throw(ServiceException&);
    
    virtual Overlay* getOverlay() = 0;

    /* Maintains the subscription over the mesh;
     * EventSubscription is a metada data place for a specific event
     */
    /*virtual void addSubscriber(DiscoveryEventSubscriber* subscriber,
                                EventSubscription* subscription) = 0;*/
    //virtual void addPublisher(DiscoveryEventPublisher* publisher) = 0;
    //Must find subscribers and send the event
    //virtual void publish(DiscoveryEvent* event)  = 0;
        
protected:
    
    Discovery(Overlay* overlay);            
    ACE_Hash_Map_Manager<UInt, DiscoveryProviderInfo*, ACE_SYNCH_RW_MUTEX> m_eventProvider;
    
    virtual DiscoveryProviderInfo* findProvider(UInt providerID);
    
    
};
typedef ACE_Strong_Bound_Ptr<Discovery, ACE_Recursive_Thread_Mutex> DiscoveryPtr;

#endif	/* DISCOVERY_H */

