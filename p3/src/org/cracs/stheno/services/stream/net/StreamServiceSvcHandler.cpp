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
 * File:   StreamServiceSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "StreamServiceSvcHandler.h"
#include <stheno/services/stream/StreamService.h>

StreamServiceSvcHandler::StreamServiceSvcHandler(StreamService* streamServer,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new StreamServicePacketFactory(), false, false,
ec, reserve, qos, closeListener) {
    m_streamServer = streamServer;
    m_lastFrameNo = 0;
}

StreamServiceSvcHandler::~StreamServiceSvcHandler() {
}

int StreamServiceSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
            //this->getPID()->toString().c_str(), m_fid->toString().c_str()));
    switch (packet->getPacketHeader()->getType()) {
            /*case P3DiscoveryQueryPacket::P3DISCOVERY_QUERY_PACKET:{
               m_discovery->handleQueryPacket(this,static_cast<P3DiscoveryQueryPacket*>(packet));
               return 0;
            }*/

        default:
        {
            delete packet;
            return -1;
        }
    }
    return -1;
}

int StreamServiceSvcHandler::open(void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: StreamServiceSvcHandler::open\n")));
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: StreamServiceSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);


    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case StartStreamServicePacket::START_STREAM_SERVICE_PACKET_OP:
        {
            StartStreamServicePacket* initSessionPacket = static_cast<StartStreamServicePacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
            m_fid = initSessionPacket->getFID();
            delete recvPacket;
            //m_frameSize = initSessionPacket->getFrameSize();
            //m_frameRate = initSessionPacket->getFrameRate();
            //int ret = m_streamServer->createSession(this);
            if (ret == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamServiceSvcHandler::open - NOK PID=%s\n"), m_pid->toString().c_str()));
                close();
            }
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler::open - OK PID=%s\n"), m_pid->toString().c_str()));
            return 0;
        }
        case ResumeStreamServicePacket::RESUME_STREAM_SERVICE_PACKET_OP:
        {
            ResumeStreamServicePacket* initSessionPacket = static_cast<ResumeStreamServicePacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler::open - ResumeStreamServicePacket PID=%s\n"), m_pid->toString().c_str()));
            m_fid = initSessionPacket->getFID();
            if (ret == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamServiceSvcHandler::open - ResumeStreamServicePacket NOK PID=%s\n"), m_pid->toString().c_str()));
                close();
            }
            list<StreamFramePacket*> frameList;
            m_streamServer->getRetrieveStorage(initSessionPacket->getLastFrameNo()+1, frameList);
            ResumeStreamReplyPacket* reply = new ResumeStreamReplyPacket(0, frameList);
            if (this->sendPacket(reply, 0) == -1) {
                delete reply;
                delete recvPacket;
                close();
                return -1;
            }
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceSvcHandler::open - ResumeStreamServicePacket OK PID=%s\n"), m_pid->toString().c_str()));
            delete reply;
            delete recvPacket;
            return 0;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamServiceSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            if(recvPacket!=0){
                delete recvPacket;
            }
            return -1;
        }
    }

}

