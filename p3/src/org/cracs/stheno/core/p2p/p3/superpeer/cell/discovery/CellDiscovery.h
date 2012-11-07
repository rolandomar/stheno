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
 * File:   CellDiscovery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 26, 2010, 3:32 PM
 */

#ifndef CELLDISCOVERY_H
#define	CELLDISCOVERY_H

#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/discovery/Discovery.h>
//#include <stheno/core/p2p/net/Network.h>

#include <euryale/net/PacketDatagramChannel.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/CellDiscoveryChannel.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/exception/CellDiscoveryException.h>

#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCellReplyPacket.h>

#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/RequestCoordinatorReply.h>


#include <stheno/core/p2p/p3/superpeer/cell/CellReply.h>
#include <stheno/core/p2p/p3/P3Config.h>
#include <euryale/common/sleep/SleepHelper.h>

class QoSManagerInterface;
class CellDiscovery  {
public:
    friend class Cell;
    CellDiscovery(UUIDPtr& uuid,QoSManagerInterface* qosManager);
    static const int REQUEST_CELL_TIMEOUT_MS = 20;
    static const int REQUEST_CELL_TIMEOUT_INTERVAL_MS = 5;
    static const int REQUEST_CELL_MAX_TRIES = 3; //MAX_TIME=2*(15+20)
    static const int MAX_WAIT_TIME = REQUEST_CELL_MAX_TRIES * (REQUEST_CELL_TIMEOUT_MS+REQUEST_CELL_TIMEOUT_INTERVAL_MS);

    static const bool DOUBLE_SEND_KERNEL_2_6_39_BUG = false;
    
    virtual ~CellDiscovery();
    //close interface
    void close();
    //requests a new Cell, Only appliable to root cell
    CellReply* requestCell(UUIDPtr& cellID);
    //every cell
    Endpoint* findCoordinator() throw (CellDiscoveryException&);    
    
    //open root discovery
    void open(Cell* cell = 0) throw (CellDiscoveryException&);
    //open a cellID discovery
    void open(CellAddrPtr& cellAddr,Cell* cell = 0)
            throw (CellDiscoveryException&);

    
    void attach(Cell* cell) throw(CellDiscoveryException&);
    void deattach();

    //handle a new discovery packet
    void handleDiscoveryPacket(SthenoPacket* packet);
    //
    Cell* getCell();            
    //Keep Alive
    void sendGroupKeepAlive();

    int asynchronous();

    QoSManagerInterface* getQoSManager();
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_debugCellDiscovery;
    CellIDPtr m_cellID;
    UUIDPtr m_uuid;
    Cell* m_cell;
    CellDiscoveryChannel* m_channel;
    ThreadPerConnection* m_tpc;
    QoSManagerInterface* m_qosManager;
    ULong m_keepAliveCounter;

    //send interface
    int sendPacket(SthenoPacket* packet, ACE_Time_Value* timeout);

};

#endif	/* CELLDISCOVERY_H */

