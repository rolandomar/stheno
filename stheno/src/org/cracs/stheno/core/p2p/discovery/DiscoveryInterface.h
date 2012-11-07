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
 * File:   DiscoveryInterface.h
 * Author: rmartins
 *
 * Created on October 25, 2010, 7:28 PM
 */

#ifndef DISCOVERYINTERFACE_H
#define	DISCOVERYINTERFACE_H

#include <stheno/core/p2p/discovery/Discovery.h>
typedef ACE_Strong_Bound_Ptr<Discovery, ACE_Recursive_Thread_Mutex> DiscoveryPtr;

class DiscoveryInterface {
public:
    friend class SthenoCore;
    virtual ~DiscoveryInterface();
    
    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0);
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0);
private:
    DiscoveryInterface(DiscoveryPtr& discoveryPTr);
    DiscoveryPtr m_discoveryPTr;
};

typedef ACE_Strong_Bound_Ptr<DiscoveryInterface, ACE_Recursive_Thread_Mutex> DiscoveryInterfacePtr;

#endif	/* DISCOVERYINTERFACE_H */

