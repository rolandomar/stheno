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
 * File:   FTControlClientHandler.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:29 AM
 */

#ifndef FTCONTROLCLIENTHANDLER_H
#define	FTCONTROLCLIENTHANDLER_H

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
#include <stheno/core/p2p/p3/ft/net/control/packet/FTControlPacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>


class P3ReplicationGroup;

class FTControlClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    typedef ChannelRequestEngine<FTControlClientHandler, SthenoStreamPacket*>::RequestPtr RequestPtr;

    FTControlClientHandler();

    FTControlClientHandler(P3ReplicationGroup* rg, UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& rgid,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~FTControlClientHandler();

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
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    int initSession(
            UUIDPtr& sid,
            UUIDPtr& iid,
            ServiceParamsPtr& params,
            OverlayPeerInfoPtr& primary,
            list<OverlayPeerInfoPtr>& group,
            SAPInfoPtr& ftDataSAPInfo);
    /**
     * Prebind a FT control connection without creating a replication group
     * @return 
     */
    int preBindSession();

    RequestPtr* addReplica(UUIDPtr& uuid, UUIDPtr& fid, OverlayPeerInfoPtr& pi) {
        FTAddReplicaPacket* addReplicaPacket = new FTAddReplicaPacket(0, uuid, fid, m_rgid, pi);
        RequestPtr* ret = sendRequest((SthenoStreamPacket*&) addReplicaPacket, 0);
        delete addReplicaPacket;
        return ret;
    }

    RequestPtr* removeReplica(UUIDPtr& uuid, UUIDPtr& fid) {
        FTRemoveReplicaPacket* removeReplicaPacket =
                new FTRemoveReplicaPacket(0, uuid, fid, m_rgid);
        RequestPtr* ret = sendRequest((SthenoStreamPacket*&) removeReplicaPacket, 0);
        delete removeReplicaPacket;
        return ret;
    }
    virtual int handlePacket(SthenoStreamPacket* packet);

    virtual ACE_Reactor *reactor(void) const;

    virtual void reactor(ACE_Reactor *reactor);

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    virtual RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (m_close) {
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestPtr* request =
                new RequestPtr(
                new ChannelRequest<FTControlClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout)
                );
        m_requests.bind(request);
        int ret = 0;
        if (this->hasAsyncWrite()) {
            WriteBlockPtr* wb = new WriteBlockPtr(new WriteBlock(mb, timeout));
            WriteBlockPtr* wbAsync = new WriteBlockPtr(*wb);
            (*request)->attachWriteBlock(wb);
            ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::sendAsync(wbAsync);
        } else {
            ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
            ACE_Message_Block::release(mb);
        }
        /*int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);*/
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlClientHandler:sendRequest() - ID(%d)\n"), id));
            delete request;
            return 0;
        }
        return request;
    }

    UUIDPtr& getPID();
    UUIDPtr& getFID();
    UUIDPtr& getRGID();

protected:
    bool m_debugFTControlClientHandler;
    P3ReplicationGroup* m_rg;
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;
    int m_receiveSequence;
    ChannelRequestEngine<FTControlClientHandler, SthenoStreamPacket*> m_requests;
};

#endif	/* FTCONTROLCLIENTHANDLER_H */

