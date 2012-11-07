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
 * File:   StreamClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "StreamClientHandler.h"

StreamClientHandler::~StreamClientHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClientHandler:~StreamClientHandler()\n")));
    close();
}

StreamClientHandler::StreamClientHandler() :
m_frameListener(0) {
    throw -1;
}

StreamClientHandler::StreamClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        int frameSize, int frameRate,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new StreamPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_frameListener(0), m_uuid(uuid), m_fid(fid), m_frameSize(frameSize), m_frameRate(frameRate) {
    //m_requests = new ChannelRequestEngine<StreamClientHandler, SthenoStreamPacket*>();
}

int StreamClientHandler::open(void *arg) {
    //InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qos);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClientHandler::open()\n")));
    m_receiveSequence = 0;
    StartStreamPacket* initPacket = new StartStreamPacket(0, m_uuid, m_fid, m_frameSize, m_frameRate);
    ACE_Time_Value* timeout = 0;
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClientHandler::open() - after send ret=%d\n"), ret));
    if (ret == -1) {
        return -1;
    }
    ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
    return ret;
}

ACE_Reactor* StreamClientHandler::reactor(void) const {
    ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));
    return reactor;
}

void StreamClientHandler::reactor(ACE_Reactor *reactor) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
}

int StreamClientHandler::handlePacket(SthenoStreamPacket* packet) {
    switch (packet->getPacketHeader()->getType()) {
        case StartStreamReplyPacket::START_STREAM_REPLY_PACKET:
        {
            StartStreamReplyPacket* replyPacket = static_cast<StartStreamReplyPacket*> (packet);
            if (m_requests.setReply(replyPacket->getPacketReplyID(), packet) == -1) {
                delete packet;
                return -1;
            }
            return 0;
        }
        case FramePacket::FRAME_PACKET_OP:
        {
            FramePacket* framePacket = static_cast<FramePacket*> (packet);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClientHandler::handlePacket() - new frame!(%d)\n"), framePacket->getPacketHeader()->getPacketID()));
            if (m_frameListener == 0) {
                delete framePacket;
            } else {
                m_frameListener->onFramePacket(framePacket);
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

ChannelRequestEngine<StreamClientHandler, SthenoStreamPacket*>::RequestPtr* StreamClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
    if (this->m_close) {
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<StreamClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<StreamClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<StreamClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout));
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

UUIDPtr& StreamClientHandler::getPID() {
    return m_uuid;
}

UUIDPtr& StreamClientHandler::getFID() {
    return m_fid;
}

void StreamClientHandler::setFrameListener(StreamFrameListener* listener) {
    m_frameListener = listener;
}
