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
 * File:   ActuatorServiceClientHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "ActuatorServiceClientHandler.h"

ActuatorServiceClientHandler::~ActuatorServiceClientHandler() {
    close();
}

ActuatorServiceClientHandler::ActuatorServiceClientHandler():
m_counter(0)
{
    throw -1;
}

ActuatorServiceClientHandler::ActuatorServiceClientHandler(
        UUIDPtr& uuid, 
        UUIDPtr& fid,                
        bool asyncRead, bool asyncWrite,        
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new ActuatorServicePacketFactory(),asyncRead, asyncWrite,
ec, reserve, qos, closeListener),         
        m_uuid(uuid), m_fid(fid),
m_counter(0)
{
    m_requests = new ChannelRequestEngine<ActuatorServiceClientHandler,SthenoStreamPacket*>();
    printf("ActuatorServiceClientHandler %p\n",this);
}
