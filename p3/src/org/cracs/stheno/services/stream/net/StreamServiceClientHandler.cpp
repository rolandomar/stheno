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
 * File:   StreamServiceClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "StreamServiceClientHandler.h"

StreamServiceClientHandler::~StreamServiceClientHandler() {
    printf("StreamServiceClientHandler::StreamServiceClientHandler\n");
    close();
}

StreamServiceClientHandler::StreamServiceClientHandler():
m_counter(0)
{
    throw -1;
}

StreamServiceClientHandler::StreamServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid,        
        bool asyncRead, bool asyncWrite,        
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new StreamServicePacketFactory(),asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid),m_lastFrameNo(0),m_frameListener(0),
m_counter(0)
{
    //m_requests = new ChannelRequestEngine<StreamServiceClientHandler,SthenoStreamPacket*>();
    printf("StreamServiceClientHandler %p\n",this);
}


StreamServiceClientHandler::StreamServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        UInt lastFrameNo,
        bool asyncRead, bool asyncWrite,        
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new StreamServicePacketFactory(),asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid),m_lastFrameNo(lastFrameNo),m_frameListener(0),
m_counter(0)
{
    //m_requests = new ChannelRequestEngine<StreamServiceClientHandler,SthenoStreamPacket*>();
    printf("StreamServiceClientHandler %p\n",this);
}