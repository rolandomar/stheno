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
 * File:   ActuatorClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "ActuatorClientHandler.h"

ActuatorClientHandler::~ActuatorClientHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorClientHandler:~ActuatorClientHandler()\n")));
    close();
}

ActuatorClientHandler::ActuatorClientHandler(){
    throw -1;
}

ActuatorClientHandler::ActuatorClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        QoSEndpoint& qosEndpoint,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new ActuatorPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid),m_qosEndpoint(qosEndpoint){
    m_requests = new ChannelRequestEngine<ActuatorClientHandler, SthenoStreamPacket*>();
}

int ActuatorClientHandler::open(void *arg) {    
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorClientHandler::open()\n")));    
    ACE_Time_Value* timeout = 0;
    InitSessionPacket* initPacket = new InitSessionPacket(0, m_uuid, m_fid, m_qosEndpoint);
    //StartStreamPacket* initPacket = new StartStreamPacket(0, m_uuid, m_fid, m_frameSize, m_frameRate);    
    int ret = sendPacket((SthenoStreamPacket*&) initPacket, timeout);
    delete initPacket;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorClientHandler::open() - after send ret=%d\n"), ret));
    if (ret == -1) {
        return -1;
    }
    return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);        
}

ACE_Reactor* ActuatorClientHandler::reactor(void) const {
    ACE_Reactor* reactor = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor() - (%@)\n"), reactor));
    return reactor;
}

void ActuatorClientHandler::reactor(ACE_Reactor *reactor) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: P3DiscoveryClientHandler::reactor(ACE_Reactor *reactor) - (%@)\n"), reactor));
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::reactor(reactor);
}

int ActuatorClientHandler::handlePacket(SthenoStreamPacket* packet) {
    switch (packet->getPacketHeader()->getType()) {
        
        case ActuatorActionReplyPacket::ACTION_REPLY_PACKET_OP:
        {
            ActuatorActionReplyPacket* actionPacket = static_cast<ActuatorActionReplyPacket*> (packet);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorClientHandler::handlePacket() - new action reply!(%d)\n"), actionPacket->getPacketHeader()->getPacketID()));
            if(m_requests->setReply(actionPacket->getRequestID(),actionPacket) == -1){
                delete actionPacket;                
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

ChannelRequestEngine<ActuatorClientHandler, SthenoStreamPacket*>::RequestPtr* ActuatorClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
    if (this->m_close) {
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<ActuatorClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<ActuatorClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<ActuatorClientHandler, SthenoStreamPacket*>(this, m_requests, id, timeout));
    m_requests->bind(request);
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
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3DiscoveryClientHandler:sendRequest() - ID(%d)\n"), id));
        delete request;
        return 0;
    }
    return request;
}

UUIDPtr& ActuatorClientHandler::getPID() {
    return m_uuid;
}

UUIDPtr& ActuatorClientHandler::getFID() {
    return m_fid;
}
