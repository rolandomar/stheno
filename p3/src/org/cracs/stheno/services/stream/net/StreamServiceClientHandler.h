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
 * File:   StreamServiceClientHandler.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:48 PM
 */

#ifndef STREAMSERVICECLIENTHANDLER_H
#define	STREAMSERVICECLIENTHANDLER_H

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
#include <stheno/services/stream/net/packet/StreamServicePacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>
#include <stheno/core/p2p/discovery/DiscoveryQoS.h>

class FrameListener {
public:
    virtual void onFrame(StreamFramePacket* frame) = 0;
};

class StreamServiceClientHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    StreamServiceClientHandler();

    StreamServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    StreamServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid, UInt lastFrameNo,
            bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    virtual ~StreamServiceClientHandler();

    void setFrameListener(FrameListener* listener) {
        m_frameListener = listener;
    }

    int open(void *arg = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if(PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg) == -1){
            printf("StreamServiceClientHandler open() -1 %p\n", this);
            return -1;
        }
        //printf("StreamServiceClientHandler open() - %p\n", this);
        if (m_lastFrameNo == 0) {
            //printf("StreamServiceClientHandler open() - 1 %p\n",this);
            StartStreamServicePacket* initPacket = new StartStreamServicePacket(0, m_uuid, m_fid);
            //printf("StreamServiceClientHandler open() - 2 %p\n",this);
            ACE_Time_Value* timeout = 0;
            //printf("StreamServiceClientHandler open() - 3 %p\n",this);
            if (sendPacket((SthenoStreamPacket*&) initPacket, timeout) == -1) {
                printf("StreamServiceClientHandler open() - %p failed with last=0\n", this);
                return -1;
            }
            //printf("StreamServiceClientHandler open() - 4 %p\n",this);
        } else {
            //printf("StreamServiceClientHandler open() - 5 %p\n",this);
            ResumeStreamServicePacket* initPacket = new ResumeStreamServicePacket(0, m_uuid, m_fid, m_lastFrameNo);
            ACE_Time_Value* timeout = 0;
            if (sendPacket((SthenoStreamPacket*&) initPacket, timeout) == -1) {
                printf("StreamServiceClientHandler open() - %p failed with last!=0\n", this);
                return -1;
            }
            //printf("StreamServiceClientHandler open() - 6 %p\n",this);
            SthenoStreamPacket* reply = 0;
            //printf("StreamServiceClientHandler open() - 7 %p before read packet\n",this);
            if (this->receivePacket(reply, 0) == -1) {
                printf("StreamServiceClientHandler open() - 8 %p read packet failed\n", this);
                return -1;
            } else {
                //printf("StreamServiceClientHandler open() - 9 %p\n",this);
                if (reply->getPacketHeader()->getType() != ResumeStreamReplyPacket::RESUME_STREAM_SERVICE_REPLY_PACKET_OP) {
                    printf("StreamServiceClientHandler open() - %p wtrong packet\n", this);
                    return -1;
                }
                //printf("StreamServiceClientHandler open() - 10 %p\n",this);
                m_frameInitList = static_cast<ResumeStreamReplyPacket*> (reply)->getFrameList();
            }
            return 0;
        }
        //printf("StreamServiceClientHandler open() - 11 %p before packet open\n",this);
        return 0;
    }

    virtual int close(u_long flags = 0) {
        {
            //printf("StreamServiceClientHandler: Closing client\n");
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

    static const int JOIN_CELL_TIMEOUT_MS = 20000;

    virtual int handlePacket(SthenoStreamPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {
            case StreamFramePacket::STREAM_FRAME_PACKET_OP:
            {
                //printf("------->StreamClientPacket: receibe FRAMRE\n");
                StreamFramePacket* framePacket = static_cast<StreamFramePacket*> (packet);
                //if (m_counter % 24 == 0) {
                ACE_Time_Value delta = ACE_OS::gettimeofday();
                delta -= framePacket->getTimestamp();
                m_counter++;
                m_totalLatency += delta;
                /*if (m_counter % 24 == 0) {
                    ULongLong deltausec;
                    //delta.to_usec(deltausec);
                    m_totalLatency.to_usec(deltausec);
                    deltausec = deltausec / m_counter;
                    ACE_DEBUG((LM_DEBUG,
                            ACE_TEXT("(%t|%T)INFO: StreamServiceClientHandler::handlePacket() - new frame!(%d) Avg lat(%Q)usec \n"),
                            framePacket->getFrameNo(),
                            deltausec
                            ));
			    } */               
                if (m_frameListener == 0) {
                    delete framePacket;
                } else {
                    m_frameListener->onFrame(framePacket);
                }
                //printf("------->StreamClientPacket: receibe FRAMRE --- FINAL\n");
                return 0;
            }
            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        return 0;
    }

    virtual ChannelRequestEngine<StreamServiceClientHandler, SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (this->m_close) {
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        ChannelRequestEngine<StreamServiceClientHandler, SthenoStreamPacket*>::RequestPtr* request =
                new ChannelRequestEngine<StreamServiceClientHandler, SthenoStreamPacket*>::RequestPtr(
                new ChannelRequest<StreamServiceClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout));
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

    list<StreamFramePacket*>& getInitFrameList() {
        return m_frameInitList;
    }

    int getCounter(){
      return m_counter;
    }

    ACE_Time_Value& getTotalLatency(){
      return m_totalLatency;
    }
protected:
    FrameListener* m_frameListener;
    UInt m_lastFrameNo;
    list<StreamFramePacket*> m_frameInitList;
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    int m_receiveSequence;
    ChannelRequestEngine<StreamServiceClientHandler, SthenoStreamPacket*> m_requests;
    //stats
    int m_counter;
    ACE_Time_Value m_totalLatency;
};

#endif	/* STREAMSERVICECLIENTHANDLER_H */

