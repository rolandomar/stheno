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
 * File:   ActuatorServiceClientHandler.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:48 PM
 */

#ifndef ActuatorServiceClientHandler_H
#define	ActuatorServiceClientHandler_H

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
#include <stheno/services/actuator/net/packet/ActuatorServicePacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>
#include <stheno/services/actuator/ServiceActuatorParams.h>
#include <stheno/services/actuator/exception/ActuatorException.h>

class ActuatorServiceClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    ActuatorServiceClientHandler();

    ActuatorServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~ActuatorServiceClientHandler();

    int open(void *arg = 0) {
        StartActuatorServicePacket* initPacket = new StartActuatorServicePacket(0, m_uuid, m_fid);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
        if (ret == -1) {
            return -1;
        }
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    }

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

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    virtual int handlePacket(SthenoStreamPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {

            case ActionReplyPacket::ACTION_REPLY_PACKET_OP:
            {
                //printf("ActuatorServiceClientHandler::handlePacket() reply\n");
                ActionReplyPacket* ar = static_cast<ActionReplyPacket*> (packet);
                if (m_requests->setReply(ar->getRequestID(), ar) == -1) {
                    printf("ActuatorServiceClientHandler::handlePacket() reply - set failed\n");
                    delete ar;
                }
                return 0;
            }
            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorServiceClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        return 0;
    }

    void action(ACE_Message_Block* arg) throw (ActuatorException&) {
        ACE_Time_Value now = ACE_OS::gettimeofday();
        ActionPacket* invocationPacket =
                new ActionPacket(
                0,
                now,
                arg
                );
        ChannelRequestEngine<ActuatorServiceClientHandler, SthenoStreamPacket*>::RequestPtr* request = 0;
        request = sendRequest(invocationPacket, 0);
        if (request == 0) {
            delete invocationPacket;
            throw ActuatorException(ActuatorException::INVALID_RUNTIME);
        }
        //printf("Wait for future\n");
        list<SthenoStreamPacket*>* l = request->get()->waitFuture(0);
        //printf("after for future\n");
        if (l == 0 || l->size() == 0) {
            delete request;
            delete invocationPacket;
            throw ActuatorException(ActuatorException::INVALID_RUNTIME);
        }
        ActionReplyPacket* replyPacket = static_cast<ActionReplyPacket*> (l->front());
        ListHelper<SthenoStreamPacket*>::deleteList(l);
        delete invocationPacket;
        delete request;
        return;
    }

    virtual ChannelRequestEngine<ActuatorServiceClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (this->m_close) {
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        ChannelRequestEngine<ActuatorServiceClientHandler, SthenoStreamPacket*>::RequestPtr* request =
                new ChannelRequestEngine<ActuatorServiceClientHandler, SthenoStreamPacket*>::RequestPtr(
                new ChannelRequest<ActuatorServiceClientHandler, SthenoStreamPacket*>(this, m_requests, id, timeout));
        m_requests->bind(request);
        //int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        //ACE_Message_Block::release(mb);
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
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler:sendRequest() - ID(%d)\n"), id));
            delete request;
            return 0;
        }
        return request;
    }

    UUIDPtr& getPID() {
        return m_uuid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    ChannelRequestEngine<ActuatorServiceClientHandler, SthenoStreamPacket*>* m_requests;
    //stats
    int m_counter;
    ACE_Time_Value m_totalLatency;
};

#endif	/* ActuatorServiceClientHandler_H */

