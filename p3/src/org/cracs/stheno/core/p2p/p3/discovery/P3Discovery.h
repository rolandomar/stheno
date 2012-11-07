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
 * File:   P3Discovery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 6, 2010, 11:14 AM
 */

#ifndef P3DISCOVERY_H
#define	P3DISCOVERY_H

#include <stheno/core/p2p/discovery/Discovery.h>
//#include <euryale/net/endpoint/SAP.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoveryAcceptor.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoverySAP.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoverySvcHandler.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoveryClientHandler.h>

#include <stheno/core/p2p/network/net/stream/StreamChannelAcceptor.h>
#include <stheno/core/p2p/network/net/stream/AsyncPacketReaderHandler.h>

/*
 * P3 Discovery Interface
 */
class P3;
class P3Mesh;
class SuperPeer;

class P3Discovery : public Discovery{
public:
    friend class P3DiscoverySvcHandler;
    friend class P3;
    friend class SuperPeer;
    virtual ~P3Discovery();

    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
            DiscoveryQoS* qos = 0) = 0;

    /*virtual AsyncDiscoveryQueryReply* executeAsyncQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos = 0) {}*/

    virtual const char* id();
    
    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);
    
    virtual void close();

    virtual Overlay* getOverlay();

    virtual ServiceAbstract* duplicate() throw (ServiceException&);
   
    virtual void getSAP(SAPInfoPtr& sapInfo);
   
    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);
  
    virtual const char* getName();
   
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    P3DiscoverySAP* m_sap;

    P3Discovery(P3* overlay);
    
    ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> m_clients;
    

    //Local execution
    virtual DiscoveryQueryReply* executeLocalQuery(DiscoveryQuery* query,
            DiscoveryQoS* qos = 0);

    virtual void handleQueryPacket(P3DiscoverySvcHandler* handler, P3DiscoveryQueryPacket* packet) = 0;

    //virtual void open_i(ServiceParamsPtr& params,int fttype) throw (ServiceException&) {}
};

#endif	/* P3DISCOVERY_H */

