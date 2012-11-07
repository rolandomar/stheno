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
 * File:   P3DiscoverySAP.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 11:36 AM
 */

#include "P3DiscoverySAP.h"
#include <stheno/core/p2p/p3/discovery/P3Discovery.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoveryAcceptor.h>


P3DiscoverySAP::P3DiscoverySAP(P3Discovery* discovery) :
m_discovery(discovery) {
}

P3DiscoverySAP::~P3DiscoverySAP() {
    close();
}

P3DiscoveryAcceptor* P3DiscoverySAP::createAcceptor() {
    return new P3DiscoveryAcceptor(m_discovery);
    //return new StreamChannelAcceptor(m_discovery);
}
