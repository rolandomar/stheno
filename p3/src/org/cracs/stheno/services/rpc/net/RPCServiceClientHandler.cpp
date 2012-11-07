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
 * File:   RPCServiceClientHandler.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:27 PM
 */

#include "RPCServiceClientHandler.h"
#include <stheno/services/rpc/RPCServiceClient.h>

RPCServiceClientHandler::RPCServiceClientHandler() :
m_qosEndpoint(Endpoint::TCP, 0) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("RPCServiceClient: RPCServiceClient::RPCServiceClient(1) %@\n"),this));    

}

RPCServiceClientHandler::~RPCServiceClientHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("RPCServiceClient: RPCServiceClient::~RPCServiceClient(1) %@\n"),this));    
    close();
}

RPCServiceClientHandler::RPCServiceClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        QoSEndpoint& qosEndpoint,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new RPCServicePacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid), m_qosEndpoint(qosEndpoint) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("RPCServiceClient: RPCServiceClient::~RPCServiceClient(2) %@\n"),this));    
    //m_requests = new ChannelRequestEngine<RPCServiceClientHandler, SthenoStreamPacket*>();    
}

int RPCServiceClientHandler::close(u_long flags) {
    {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    if (!m_close) {
        /*if (m_requests != 0) {
            printf("RPCServiceClientHandler::close() before delet map\n");
            delete m_requests;
            m_requests = 0;
        }*/
        m_requests.clearAll();
    }
    }
    return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
}

