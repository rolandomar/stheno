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
 * File:   RPCServiceClientHandler.h
 * Author: rmartins
 *
 * Created on January 21, 2011, 4:27 PM
 */

#ifndef RPCSERVICECLIENTHANDLER_H
#define	RPCSERVICECLIENTHANDLER_H

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

#include <euryale/net/ChannelRequestEngine.h>

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>

#include <stheno/services/rpc/exception/RPCException.h>
#include <stheno/services/rpc/net/packet/RPCServicePacketFactory.h>
#include <stheno/Stheno.h>

class RPCServiceClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    RPCServiceClientHandler();

    RPCServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid, QoSEndpoint& qosEndpoint,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    virtual ~RPCServiceClientHandler();

    virtual int close(u_long flags = 0);

    int open(void *arg = 0) {
        //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qosEndpoint);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
        delete initPacket;
        if (ret == -1) {
            printf("RPCServiceClientHandler: open failed on sendpacket\n");
            return -1;
        }
        ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        if (ret == -1) {
            printf("RPCServiceClientHandler: open failed on super\n");
        }
        return ret;
    }

    ACE_INLINE ACE_Message_Block* twoWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) throw (RPCException&) {
        /*ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock,0);
        if(m_close){
            return 0;
        }*/
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        ACE_Time_Value now = ACE_OS::gettimeofday();
        RPCInvocationPacket* invocationPacket =
                new RPCInvocationPacket(
                0,
                oid,
                arg,
                now
                );
        
        int ret = this->sendPacket(invocationPacket, 0);
        //ACE_Time_Value end = ACE_OS::gettimeofday() - now;
        //ULongLong usec;
        //end.to_usec(usec);
        //printf("twoWayInvocation after sned time=%d\n", usec);
        if (ret == -1) {
            delete invocationPacket;
            throw RPCException(RPCException::INVALID_RUNTIME);
        }
        SthenoStreamPacket* replyPacketMeta = 0;
        
        ret = this->receivePacket(replyPacketMeta, 0);
        if (ret == -1) {
            delete invocationPacket;
            throw RPCException(RPCException::INVALID_RUNTIME);
        }
        RPCInvocationReplyPacket* replyPacket = static_cast<RPCInvocationReplyPacket*> (replyPacketMeta);
        ACE_Message_Block* payload = replyPacket->getPayload()->duplicate();
        delete invocationPacket;
                //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
            //ULongLong usec;
            
            //end.to_usec(usec);
            //printf("RPC=%ld\n",usec);
                    

        return payload;
        /*ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*>::RequestPtr* request = 0;
        request = sendRequest(invocationPacket, 0);
        if (request == 0) {
            delete invocationPacket;
            throw RPCException(RPCException::INVALID_RUNTIME);
        }
        list<SthenoStreamPacket*>* l = request->get()->waitFuture(0);
        if (l == 0 || l->size() == 0) {
            delete request;
            delete invocationPacket;
            throw RPCException(RPCException::INVALID_RUNTIME);
        }
        RPCInvocationReplyPacket* replyPacket = static_cast<RPCInvocationReplyPacket*> (l->front());
        ACE_Message_Block* payload = replyPacket->getPayload()->duplicate();
        ListHelper<SthenoStreamPacket*>::deleteList(l);
        delete invocationPacket;        
        //(*request)->close();
        delete request;
        return payload;*/
    }

    void oneWayInvocation(UUIDPtr& oid, ACE_Message_Block* arg) throw (RPCException&) {
        //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock);
        ACE_Time_Value now = ACE_OS::gettimeofday();
        RPCInvocationOneWayPacket* invocationPacket =
                new RPCInvocationOneWayPacket(
                0,
                oid,
                arg,
                now
                );
        printf("Sending one way invocation!\n");
        if (this->sendPacket(invocationPacket, 0) != 0) {
            delete invocationPacket;
            throw RPCException(RPCException::INVALID_RUNTIME);
        }
        delete invocationPacket;
    }

    virtual int handlePacket(SthenoStreamPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {
            case RPCInvocationReplyPacket::RPC_INVOCATION_REPLY_PACKET_OP:
            {
                RPCInvocationReplyPacket* invocationReplyPacket = static_cast<RPCInvocationReplyPacket*> (packet);
                //printf("Received reply to invocation %p\n", invocationReplyPacket->getPayload());
                if (m_requests.setReply(invocationReplyPacket->getRequestID(), packet) == -1) {
                    delete invocationReplyPacket;
                    printf("failed to set reply!\n");
                }
                return 0;
            }
            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        return 0;
    }

    virtual ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (this->m_close) {
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*>::RequestPtr* request =
                new ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*>::RequestPtr(
                new ChannelRequest<RPCServiceClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout));
        m_requests.bind(request);
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
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
    QoSEndpoint m_qosEndpoint;

    ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*> m_requests;
};


#endif	/* RPCSERVICECLIENTHANDLER_H */

