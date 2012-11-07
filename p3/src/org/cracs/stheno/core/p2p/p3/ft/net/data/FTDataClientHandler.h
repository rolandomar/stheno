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
 * File:   FTDataClientHandler.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:29 AM
 */

#ifndef FTDataClientHandler_H
#define	FTDataClientHandler_H

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
#include <stheno/core/p2p/p3/ft/net/data/packet/FTDataPacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>


class P3ReplicationGroup;

class FTDataClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr RequestPtr;


    FTDataClientHandler();

    FTDataClientHandler(P3ReplicationGroup* rg, UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& rgid,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~FTDataClientHandler();

    virtual int close(u_long flags = 0) {
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
            if (!m_close) {
                if (m_requests != 0) {
                    delete m_requests;
                    m_requests = 0;
                }
            }
        }
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    int open(void *arg = 0) {
        //InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qos);
        m_receiveSequence = 0;
        /*StartStreamServicePacket* initPacket = new StartStreamServicePacket(0, m_uuid, m_fid);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
        if (ret == -1) {
            return -1;
        }
        ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
        return ret;*/
        return 0;
    }

    RequestPtr* replicate(ACE_Message_Block* payload) {
        /*if (m_debugFTDataClientHandler) {
            printf("<<<<<<<<<<<<<<<<<<<<<<Replicate>>>>>>>>>>>>>>>>>>>\n");
        }*/
        //payload is freed upon packet deletion
        FTDataReplicationPacket* replicatePacket = new FTDataReplicationPacket(0, payload);
        RequestPtr* ret = sendRequest((SthenoStreamPacket*&) replicatePacket, 0);
        delete replicatePacket;
        return ret;
    }


    int initSession();
    int preBindSession();

    virtual ACE_Reactor *reactor(void) const {
        ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));
        return reactor;
    }

    virtual void reactor(ACE_Reactor *reactor) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
        PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
    }

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    virtual int handlePacket(SthenoStreamPacket* packet) {
        if (m_debugFTDataClientHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataClientHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                    this->getPID()->toString().c_str(), m_fid->toString().c_str()));
        }
        switch (packet->getPacketHeader()->getType()) {
            case FTDataReplicationReplyPacket::DATA_REPLICATION_REPLY_PACKET_OP:
            {
                FTDataReplicationReplyPacket* replicationReplyPacket = static_cast<FTDataReplicationReplyPacket*> (packet);
                int ret = m_requests->setReply(replicationReplyPacket->getPacketReplyID(), replicationReplyPacket);
                if (ret == -1) {
                    delete replicationReplyPacket;
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler::handlePacket() - set reply failed(%d)\n"), packet->getPacketHeader()->getType()));
                }
                return 0;
            }

            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        return 0;
    }

    virtual ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0);

    UUIDPtr& getPID() {
        return m_uuid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    UUIDPtr& getRGID() {
        return m_rgid;
    }

protected:
    bool m_debugFTDataClientHandler;
    P3ReplicationGroup* m_rg;
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    UUIDPtr m_rgid;
    int m_receiveSequence;
    ChannelRequestEngine<FTDataClientHandler, SthenoStreamPacket*>* m_requests;
};

#endif	/* FTDataClientHandler_H */

