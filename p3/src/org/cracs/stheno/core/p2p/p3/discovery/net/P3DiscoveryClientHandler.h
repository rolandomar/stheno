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
 * File:   P3DiscoveryClientHandler.h
 * Author: rmartins
 *
 * Created on September 20, 2010, 12:21 PM
 */

#ifndef P3DISCOVERYCLIENTHANDLER_H
#define	P3DISCOVERYCLIENTHANDLER_H

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
#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryPacketFactory.h>
//#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
//#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <euryale/net/ChannelRequestEngine.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>

class P3DiscoveryClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
//P3DiscoveryPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    P3DiscoveryClientHandler();

    P3DiscoveryClientHandler(UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qosE, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~P3DiscoveryClientHandler();

    int open(void *arg = 0);

    virtual int close(u_long flags = 0) {
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
            if (!m_close) {
                /*if (m_requests != 0) {
                    delete m_requests;
                    m_requests = 0;
                }*/
                m_requests.clearAll();
            }
        }
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader, ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual ACE_Reactor *reactor(void) const;
    virtual void reactor(ACE_Reactor *reactor);

    virtual DiscoveryQueryReply* executeQuery(DiscoveryQuery* query,
            DiscoveryQoS* qos, ACE_Time_Value* timeout);

    virtual int handlePacket(SthenoStreamPacket* packet);
    virtual ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0);

    UUIDPtr& getPID();
    UUIDPtr& getFID();
    QoSEndpoint& getQoSEndpoint();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    QoSEndpoint m_qos;
    ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*> m_requests;
};


#endif	/* P3DISCOVERYCLIENTHANDLER_H */

