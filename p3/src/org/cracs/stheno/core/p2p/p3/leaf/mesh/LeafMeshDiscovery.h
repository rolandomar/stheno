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
 * File:   LeafDiscovery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 6, 2010, 11:20 AM
 */

#ifndef LEAFDISCOVERY_H
#define	LEAFDISCOVERY_H

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/CellDiscoveryChannel.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/exception/CellDiscoveryException.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorReply.h>

#include <stheno/core/p2p/p3/P3Config.h>
#include <euryale/common/sleep/SleepHelper.h>

#include <stheno/core/p2p/p3/leaf/mesh/CoordinatorInfo.h>

class LeafMeshDiscovery {
public:
    friend class Cell;
    //static const int REQUEST_CELL_TIMEOUT_MS = 5;
    static const int REQUEST_CELL_TIMEOUT_MS = 55;
    //static const int REQUEST_CELL_TIMEOUT_MS = 150;
    //static const int REQUEST_CELL_TIMEOUT_MS = 250;
    //static const int REQUEST_CELL_TIMEOUT_MS = 5000;
    //static const int REQUEST_CELL_TIMEOUT_INTERVAL_MS = 3;
    //static const int REQUEST_CELL_TIMEOUT_INTERVAL_MS = 25;
    static const int REQUEST_CELL_TIMEOUT_INTERVAL_MS = 50;
    static const int REQUEST_CELL_MAX_TRIES = 10; //MAX_TIME=2*(15+20)
    static const int MAX_WAIT_TIME = REQUEST_CELL_MAX_TRIES * (REQUEST_CELL_TIMEOUT_MS + REQUEST_CELL_TIMEOUT_INTERVAL_MS);

    LeafMeshDiscovery(UUIDPtr& uuid);
    virtual ~LeafMeshDiscovery();
    //close interface
    void close();
    void open() throw (CellDiscoveryException&);

    //Request a coordinator to bind to
    CoordinatorInfo* requestCoordinator() throw (CellDiscoveryException&);

    //handle a new discovery packet
    //void handleDiscoveryPacket(SthenoPacket* packet);

    //void asynchronous();

protected:    
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    UUIDPtr m_uuid;
    CellDiscoveryChannel* m_channel;
    //ThreadPerConnection* m_tpc;

    //send interface
    int sendPacket(SthenoPacket* packet, ACE_Time_Value* timeout);
};

#endif	/* LEAFDISCOVERY_H */

