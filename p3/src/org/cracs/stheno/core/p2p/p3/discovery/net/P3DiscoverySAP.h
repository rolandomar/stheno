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
 * File:   P3DiscoverySAP.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 11:36 AM
 */

#ifndef P3DISCOVERYSAP_H
#define	P3DISCOVERYSAP_H

#include <euryale/net/endpoint/SAP.h>

class P3DiscoveryAcceptor;
class P3Discovery;
#include <stheno/core/p2p/network/net/stream/StreamChannelAcceptor.h>
class P3DiscoverySAP: public SAP<P3DiscoveryAcceptor> {
//class P3DiscoverySAP: public SAP<StreamChannelAcceptor> {
public:
    P3DiscoverySAP(P3Discovery* discovery);    
    virtual ~P3DiscoverySAP();
    
protected:
    P3Discovery* m_discovery;    
    //virtual abstract method impl
    P3DiscoveryAcceptor* createAcceptor();

};

#endif	/* P3DISCOVERYSAP_H */

