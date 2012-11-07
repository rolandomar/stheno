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
 * File:   ActuatorSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "ActuatorSvcHandler.h"
#include <stheno/core/p2p/p3/leaf/actuator/ActuatorServer.h>

ActuatorSvcHandler::~ActuatorSvcHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler:~ActuatorSvcHandler()\n")));
}

int ActuatorSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
            this->getPID()->toString().c_str(), m_fid->toString().c_str()));*/
    switch (packet->getPacketHeader()->getType()) {
        case ActuatorActionPacket::ACTION_PACKET_OP:
        {
            //printf("Received aciton packet\n");
            ActuatorActionPacket* actionPacket = static_cast<ActuatorActionPacket*> (packet);
            bool acception = m_streamServer->handleAction(actionPacket->getPayload()->duplicate());
            ActuatorActionReplyPacket* replyPacket = new ActuatorActionReplyPacket(
                    0,
                    actionPacket->getPacketHeader()->getPacketID(),
                    actionPacket->getTimestamp(),
                    acception
                    );
            int ret = this->sendPacket(replyPacket, 0);
            delete replyPacket;
            delete actionPacket;
            return ret;
        }
        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler:handlePacket() - Unkonwn packet ID=%d UUID=%s FID=%s\n"),
                    packet->getPacketHeader()->getPacketID(),
                    this->getPID()->toString().c_str(), m_fid->toString().c_str()));
            delete packet;
            return -1;
        }
    }
    return -1;
}

int ActuatorSvcHandler::open(void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: ActuatorSvcHandler::open\n")));
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: ActuatorSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {
            InitSessionPacket* initSessionPacket = static_cast<InitSessionPacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
            m_fid = initSessionPacket->getFID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorSvcHandler::open - OK PID=%s\n"), m_pid->toString().c_str()));
            return 0;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            return -1;
        }
    }
}

ActuatorSvcHandler::ActuatorSvcHandler(ActuatorServer* streamServer, //bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new ActuatorPacketFactory(), false, false,
ec, reserve, qos, closeListener) {
    m_streamServer = streamServer;
}

UUIDPtr& ActuatorSvcHandler::getPID() {
    return m_pid;
}

UUIDPtr& ActuatorSvcHandler::getFID() {
    return m_fid;
}

