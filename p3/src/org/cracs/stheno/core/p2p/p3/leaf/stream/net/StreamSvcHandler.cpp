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
 * File:   StreamSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 17, 2010, 6:48 PM
 */

#include "StreamSvcHandler.h"
#include <stheno/core/p2p/p3/leaf/stream/StreamServer.h>

StreamSvcHandler::~StreamSvcHandler() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamSvcHandler:~StreamSvcHandler()\n")));
}

int StreamSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
            this->getPID()->toString().c_str(), m_fid->toString().c_str()));
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

int StreamSvcHandler::open(void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: StreamSvcHandler::open\n")));
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: StreamSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case StartStreamPacket::START_STREAM_PACKET_OP:
        {
            StartStreamPacket* initSessionPacket = static_cast<StartStreamPacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
            m_fid = initSessionPacket->getFID();
            m_frameSize = initSessionPacket->getFrameSize();
            m_frameRate = initSessionPacket->getFrameRate();
            int ret = m_streamServer->createSession(this);
            if (ret == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamSvcHandler::open - NOK PID=%s\n"), m_pid->toString().c_str()));
                close();
            }
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamSvcHandler::open - OK PID=%s\n"), m_pid->toString().c_str()));
            return ret;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: StreamSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            return -1;
        }
    }
}

StreamSvcHandler::StreamSvcHandler(StreamServer* streamServer, //bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new StreamPacketFactory(), false, false,
ec, reserve, qos, closeListener) {
    m_streamServer = streamServer;
}

int StreamSvcHandler::sendFrame(ULong frameNo, ACE_Message_Block* payload) {
    FramePacket* framePacket = new FramePacket(0, frameNo, m_frameSize, payload);
    int ret = this->sendPacket((SthenoStreamPacket*) framePacket);    
    delete framePacket;
    return ret;
}

UUIDPtr& StreamSvcHandler::getPID() {
    return m_pid;
}

UUIDPtr& StreamSvcHandler::getFID() {
    return m_fid;
}

int StreamSvcHandler::getFrameSize() {
    return m_frameSize;
}

int StreamSvcHandler::getFrameRate() {
    return m_frameRate;
}