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
 * File:   ServiceDiscoveryProvider.h
 * Author: rmartins
 *
 * Created on August 2, 2010, 5:39 PM
 */

#ifndef SERVICEDISCOVERYPROVIDER_H
#define	SERVICEDISCOVERYPROVIDER_H

#include <stheno/core/p2p/discovery/DiscoveryProvider.h>

class Discovery;

class ServiceDiscoveryProvider: public DiscoveryProvider {
public:
    /*enum{
        SERVICE_QUERY =         0x34FACB32,
        SERVICE_QUERY_REPLY =   0xDA908912
    };*/
    
    static const UInt SERVICE_PROVIDER_TYPE;
    
    
    ServiceDiscoveryProvider(Discovery* discovery);    
    virtual ~ServiceDiscoveryProvider();
    
    
    virtual void close();
    
    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) throw (DiscoveryException&);
    
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(
        DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0,
        ACE_Time_Value* timeout = 0) throw (DiscoveryException&);        
    
    //caller must free list
    virtual list<UInt>& getProvidedEvents();
    
    virtual UInt getProviderID(){
        return SERVICE_PROVIDER_TYPE;
    }
    
    
protected:    
    list<UInt> m_providedEvents;
    Discovery* m_discovery;
    
  
        
    /*virtual DiscoveryQueryReply* removeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) throw (DiscoveryException&) = 0;
    virtual AsyncDiscoveryQueryReply* 
        remoteAsyncQuery(
        DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0,
        ACE_Time_Value* timeout = 0) throw (DiscoveryException&) = 0;*/
    
};

#endif	/* SERVICEDISCOVERYPROVIDER_H */

