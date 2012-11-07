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
 * File:   SuperPeerDiscovery.h
 * Author: rmartins
 *
 * Created on October 14, 2010, 5:49 PM
 */

#ifndef SUPERPEERDISCOVERY_H
#define	SUPERPEERDISCOVERY_H


#include <stheno/core/p2p/p3/discovery/P3Discovery.h>

class P3;
class SuperPeer;
class SuperPeerDiscovery: public P3Discovery {
public:
    SuperPeerDiscovery(P3* overlay);    
    virtual ~SuperPeerDiscovery();
    
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    
    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0);
    virtual AsyncDiscoveryQueryReply* executeAsyncQuery(DiscoveryQuery* query, 
        DiscoveryQoS* qos = 0) {return 0;}
    
    SuperPeer* getOverlay_i();        
    
protected:
    bool m_debugSuperPeerDiscovery;
    void handleQueryPacket(P3DiscoverySvcHandler* handler,P3DiscoveryQueryPacket* packet);
    //void handleQueryReplyPacket(P3DiscoveryClientHandler* handler,P3DiscoveryQueryReplyPacket* packet);
    virtual DiscoveryQueryReply* executeLocalQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0);

    
};

#endif	/* SUPERPEERDISCOVERY_H */

