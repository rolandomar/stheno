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
 * File:   P3MeshClientHandler.h
 * Author: rmartins
 *
 * Created on September 30, 2010, 6:25 PM
 */

#ifndef P3MESHCLIENTHANDLER_H
#define	P3MESHCLIENTHANDLER_H

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
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshPacketFactory.h>
//#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
//#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <euryale/net/ChannelRequestEngine.h>

#include <stheno/service/ServiceParams.h>

class P3MeshClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
//P3MeshPacketFactory, 
        ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    P3MeshClientHandler();

    P3MeshClientHandler(UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qosE, bool asyncRead, bool asyncWrite,            
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~P3MeshClientHandler();

    int open(void *arg = 0) {
        InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qos);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
        if (ret == -1) {
            return -1;
        }
        ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader,//P3MeshPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
        return ret;
    }
    
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
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual ACE_Reactor *reactor(void) const {
        ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));
        return reactor;
    }

    virtual void reactor(ACE_Reactor *reactor) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
        PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
    }

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    UUIDPtr& getPID() {
        return m_uuid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    int createService(ServiceParamsPtr& params,UUIDPtr& iid);
    void removeService(UUIDPtr& sid,UUIDPtr& iid);



protected:
    bool m_debugP3MeshClientHandler;
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    QoSEndpoint m_qos;
    ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*> m_requests;
    //Cell* m_cell;

    virtual int handlePacket(SthenoStreamPacket* packet);

    virtual ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0);

};

#endif	/* P3MESHCLIENTHANDLER_H */

