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
 * File:   ActuatorServiceSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "ActuatorServiceSvcHandler.h"
#include <stheno/services/actuator/ActuatorService.h>
#include <stheno/services/actuator/net/packet/ActuatorServicePacketFactory.h>

ActuatorServiceSvcHandler::ActuatorServiceSvcHandler(ActuatorService* streamServer,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new ActuatorServicePacketFactory(), false, false,
ec, reserve, qos, closeListener) {
    m_streamServer = streamServer;
}

ActuatorServiceSvcHandler::~ActuatorServiceSvcHandler() {
}

int ActuatorServiceSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorServiceSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
            this->getPID()->toString().c_str(), m_fid->toString().c_str()));*/
    switch (packet->getPacketHeader()->getType()) {
        case ActionPacket::ACTION_PACKET_OP:
        {
            ActionPacket* ap = static_cast<ActionPacket*> (packet);
            bool acception = m_streamServer->handleActionPacket(ap);
            ActionReplyPacket* arp = new ActionReplyPacket(0, ap->getPacketHeader()->getPacketID(), ap->getTimestamp(), acception);
            if (sendPacket(arp, 0) == -1) {
                delete ap;
                delete arp;
                return -1;
            }
            //printf("SentAction Reply\n");
            delete ap;
            delete arp;
            return 0;
        }

        default:
        {
            delete packet;
            return -1;
        }
    }
    return -1;
}

int ActuatorServiceSvcHandler::open(void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: ActuatorServiceSvcHandler::open\n")));
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: ActuatorServiceSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);


    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ActuatorServiceSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case StartActuatorServicePacket::START_ACTUATOR_SERVICE_PACKET_OP:
        {
            StartActuatorServicePacket* initSessionPacket = static_cast<StartActuatorServicePacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            m_fid = initSessionPacket->getFID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorServiceSvcHandler::open() - svc is open!\n")));
            return 0;
        }
        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ActuatorServiceSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            if (recvPacket != 0) {
                delete recvPacket;
            }
            return -1;
        }
    }

}

