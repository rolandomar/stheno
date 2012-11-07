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
 * File:   DiscoveryProvider.h
 * Author: rmartins
 *
 * Created on July 26, 2010, 5:02 PM
 */

#ifndef DISCOVERYPROVIDER_H
#define	DISCOVERYPROVIDER_H

#include <stheno/core/p2p/discovery/DiscoveryQoS.h>
#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/discovery/AsyncDiscoveryQueryReply.h>
#include <stheno/core/p2p/discovery/exception/DiscoveryException.h>
#include <list>
using std::list;

class DiscoveryProvider {
public:
    DiscoveryProvider();
    virtual ~DiscoveryProvider();
    virtual void close() = 0;
    //sync
    virtual DiscoveryQueryReply* executeQuery(
            DiscoveryQuery* query,
            DiscoveryQoS* qos = 0) throw (DiscoveryException&) = 0;
    //async
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(
            DiscoveryQuery* query,
            DiscoveryQoS* qos = 0) throw (DiscoveryException&) = 0;
    virtual void cancelAsyncQuery(AsyncDiscoveryQueryReply* token) throw (DiscoveryException&) = 0;

    virtual UInt getProviderID() = 0;
    
    virtual list<UInt>& getProvidedEvents() = 0;

protected:
    bool isEventProvided(UInt event);
    
};

#endif	/* DISCOVERYPROVIDER_H */

