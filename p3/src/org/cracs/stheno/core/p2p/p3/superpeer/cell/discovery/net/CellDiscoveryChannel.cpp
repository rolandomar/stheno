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
 * File:   CellDiscovery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 25, 2010, 4:21 PM
 */

#include "CellDiscoveryChannel.h"
#include <stheno/core/p2p/p3/superpeer/cell/discovery/CellDiscovery.h>

CellDiscoveryChannel::CellDiscoveryChannel(CellDiscovery* cellDiscovery, bool asyncRead, bool asyncWrite,
        bool inputQueue,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseDatagramListener<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>* closeListener) :
SthenoMulticastChannel<P3CellDiscoveryPacketFactory>(asyncRead, asyncWrite, inputQueue, ec, reserve, qos, closeListener),
m_cellDiscovery(cellDiscovery) {
    m_debugCellDiscoveryChannel = true; //RuntimeConfiguration::isClassDebugged("CellDiscoveryChannel");    
    //m_requests = new RequestEngine<SthenoPacket*>();
}

RequestEngine<SthenoPacket*>::RequestPtr* CellDiscoveryChannel::sendRequest(SthenoPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    if (m_closed) {
        return 0;
    }
    ACE_Message_Block* mb = serializePacket(packet);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)sendRequest=%lu %d\n", mb->length(), packet->getSerializationSize());
    UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)->REgister ID(%d)\n",id);
    //ACE_Time_Value* timeout = new ACE_Time_Value(ACE_OS::gettimeofday());
    //timeout+=5;
    RequestEngine<SthenoPacket*>::RequestPtr* request =
            new RequestEngine<SthenoPacket*>::RequestPtr(new Request<SthenoPacket*>(&m_requests, id, timeout));
    m_requests.bind(request);
    int ret = AbstractDatagramChannel<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>::send(mb, 0, timeout);
    ACE_Message_Block::release(mb);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel: sendRequest: failed!\n")));
        delete request;
        return 0;
    }
    return request;
}

CellDiscoveryChannel::~CellDiscoveryChannel() {
    close();
}

int CellDiscoveryChannel::handlePacket(SthenoPacket* packet) {
    //if (m_debugCellDiscoveryChannel) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel::handlePacket\n")));
    //}
    UUIDPtr runtimeUUID;
    m_cellDiscovery->getCell()->getPID(runtimeUUID);
    UUID* pid = runtimeUUID.get();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscoveryChannel::handlePacket 2\n");
    UUID* packetPID = ((SthenoHeader*) (packet->getPacketHeader()))->m_srcPID.get();
    if (packetPID == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel:handlePacket() - packetPID is 0!\n")));
        delete packet;
        return 0;
    }

    if (*pid == *packetPID) {
        //if (m_debugCellDiscoveryChannel) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel:handlePacket() - Our packet! Delete\n")));
        //}
        delete packet;
        return 0;
    }

    if (packet->getPacketHeader()->getDstPID().null()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel:handlePacket() - DST PID is 0!\n")));
        delete packet;
        return 0;
    }
    UUID* dstPID = packet->getPacketHeader()->getDstPID().get();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscoveryChannel::handlePacket our=%s dstPID=%s\n",pid->toString().c_str(),dstPID->toString().c_str());
    if (*dstPID == *pid) {
        UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscoveryChannel::handlePacket not request ID(%d)\n", id);
        if (m_requests.setReply(id, packet) == -1) {
            if (m_debugCellDiscoveryChannel) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel::handlePacket: out packet but request was not found!\n")));
            }
            delete packet;
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscoveryChannel::handlePacket: out packet with request!\n");
        return 0;
    }

    m_cellDiscovery->handleDiscoveryPacket(packet);
    if (m_debugCellDiscoveryChannel) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscoveryChannel::handlePacket incoming\n")));
    }
    return 0;
}