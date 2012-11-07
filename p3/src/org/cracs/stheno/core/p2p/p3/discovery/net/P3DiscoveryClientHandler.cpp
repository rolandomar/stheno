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
 * File:   P3DiscoveryClientHandler.cpp
 * Author: rmartins
 * 
 * Created on September 20, 2010, 12:21 PM
 */

#include "P3DiscoveryClientHandler.h"

P3DiscoveryClientHandler::~P3DiscoveryClientHandler() {
    close();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler:~P3DiscoveryClientHandler() %@\n"), this));
}

P3DiscoveryClientHandler::P3DiscoveryClientHandler() {
    throw -1;
}

P3DiscoveryClientHandler::P3DiscoveryClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        QoSEndpoint& qosE, bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new P3DiscoveryPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid), m_qos(qosE) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler:P3DiscoveryClientHandler() %@\n"), this));
    //m_requests = new ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>();
}

int P3DiscoveryClientHandler::open(void *arg) {
    InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qos);
    ACE_Time_Value* timeout = 0;
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
    delete initPacket;
    if (ret == -1) {
        return -1;
    }
    ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::open() - (%d) reactor(%@)\n"), ret, this->reactor()));
    return ret;
}

ACE_Reactor* P3DiscoveryClientHandler::reactor(void) const {
    ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));
    return reactor;
}

void P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
}

DiscoveryQueryReply* P3DiscoveryClientHandler::executeQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
    if(m_close){
        return 0;
    }
    P3DiscoveryQueryPacket* queryPacket = new P3DiscoveryQueryPacket(0, getPID(),
            getFID(), getQoSEndpoint(), query);
    ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            sendRequest(queryPacket, timeout);
    list<SthenoStreamPacket*>* future = (*request)->waitFuture(timeout);
    SthenoStreamPacket* reply = future->front();
    if (reply->getPacketHeader()->getType() != P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET) {
        return 0;
    }
    P3DiscoveryQueryReplyPacket* replyPacket = static_cast<P3DiscoveryQueryReplyPacket*> (reply);
    //return replyPacket->getQuery().release();
    DiscoveryQueryReply* replyQuery = replyPacket->deAttachQuery();
    delete replyPacket;
    return replyQuery;
}

int P3DiscoveryClientHandler::handlePacket(SthenoStreamPacket* packet) {
    //UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellClientHandler::handlePacket request ID(%d)\n", id);
    //if (m_requests->setReply(id, packet) == -1) {
    switch (packet->getPacketHeader()->getType()) {
        case P3DiscoveryQueryReplyPacket::P3DISCOVERY_QUERY_REPLY_PACKET:
        {
            P3DiscoveryQueryReplyPacket* replyPacket = static_cast<P3DiscoveryQueryReplyPacket*> (packet);
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
            if (m_requests.setReply(replyPacket->getPacketReplyID(), packet) == -1) {
                delete packet;
                return -1;
            }
            return 0;
        }

        default:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
            delete packet;
            return 0;
    }
    //}
    return 0;
}

ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr* P3DiscoveryClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
    if(m_close){
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<P3DiscoveryClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout));
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

UUIDPtr& P3DiscoveryClientHandler::getPID() {
    return m_uuid;
}

UUIDPtr& P3DiscoveryClientHandler::getFID() {
    return m_fid;
}

QoSEndpoint& P3DiscoveryClientHandler::getQoSEndpoint() {
    return m_qos;
}