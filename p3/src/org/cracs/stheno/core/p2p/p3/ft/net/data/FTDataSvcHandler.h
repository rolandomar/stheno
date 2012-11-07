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
 * File:   FTDataSvcHandler.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:28 AM
 */

#ifndef FTDataSvcHandler_H
#define	FTDataSvcHandler_H

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Reactor.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Stream.h"
#include "ace/LSOCK_Connector.h"
#include "ace/Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/net/PacketChannel.h>
#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/net/packet/InitSessionPacket.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>

#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataPacketFactory.h>

class P3FaultTolerance;
class P3ReplicationGroup;
class FTDataSvcHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ACE_Strong_Bound_Ptr<FTDataSvcHandler, 
        ACE_Recursive_Thread_Mutex> channel_ptr;   
    
    static const int MAX_OPEN_TIMEOUT_MS = 2000;

    FTDataSvcHandler(P3ReplicationGroup* rg, //bool asyncRead, bool asyncWrite,
                   ExecutionModelPtr* ec = 0,
                   NetReservation* reserve = 0,
                   NetQoS* qos = 0,
                   CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0) :
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, 
    ACE_SOCK_Stream, ACE_MT_SYNCH>(new FTDataPacketFactory(),false, false, 
    ec, reserve, qos, closeListener),m_replicationGroup(rg)/*,m_requests(0)*/ {
        m_debugFTDataSvcHandler = RuntimeConfiguration::isClassDebugged("FTDataSvcHandler");
        //m_ft = ft;
        //m_requests = new RequestEngine<SthenoStreamPacket*>();
    }
    
    virtual ~FTDataSvcHandler();

    virtual int open(void *arg = 0);

    virtual int close(u_long flags = 0) {
        int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);        
        return ret;
    }

    virtual int handlePacket(SthenoStreamPacket* packet);
       
    
    UUIDPtr& getPID() {
        return m_pid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }
    
    UUIDPtr& getRGID() {
        return m_rgid;
    }
                       
protected:    
    bool m_debugFTDataSvcHandler;
    P3ReplicationGroup* m_replicationGroup;    
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;    
};



#endif	/* FTDataSvcHandler_H */

