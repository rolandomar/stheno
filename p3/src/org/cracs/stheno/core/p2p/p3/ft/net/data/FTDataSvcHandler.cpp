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
 * File:   FTDataSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:28 AM
 */

#include "FTDataSvcHandler.h"
#include "packet/FTInitDataGroupPacket.h"
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

FTDataSvcHandler::~FTDataSvcHandler() {
}

int FTDataSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    if (m_debugFTDataSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: \n\n\n\nFTDataSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_fid->toString().c_str()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case FTDataReplicationPacket::DATA_REPLICATION_PACKET_OP:
        {
            FTDataReplicationPacket* replicationPacket = static_cast<FTDataReplicationPacket*> (packet);
            int ret = m_replicationGroup->handleReplicationPacket(this, replicationPacket);
            if (ret == -1) {
                delete replicationPacket;
                close();
                return -1;
            }
            FTDataReplicationReplyPacket* replyInvocationPacket =
                    new FTDataReplicationReplyPacket(0, replicationPacket->getPacketHeader()->getPacketID(), true);
            ret = this->sendPacket(replyInvocationPacket, 0);
            delete replyInvocationPacket;
            delete replicationPacket;
            return ret;
        }
        default:
        {
            delete packet;
            return -1;
        }
    }
    return -1;
}

int FTDataSvcHandler::open(void *arg) {
    if (m_debugFTDataSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: FTDataSvcHandler::open\n")));
    }
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: FTDataSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

    SthenoStreamPacket* recvPacket = 0;
    if (m_debugFTDataSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - wait for packet..\n")));
    }
    receivePacket(recvPacket, &receiveTimeout);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - after wait for packet..\n")));
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTDataSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case FTInitDataGroupPacket::INIT_DATA_GROUP_PACKET_OP:
        {
            if (m_debugFTDataSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - INCOMING PACKET\n")));
            }
            FTInitDataGroupPacket* initPacket = static_cast<FTInitDataGroupPacket*> (recvPacket);
            m_pid = initPacket->getUUID();
            m_fid = initPacket->getFID();
            m_rgid = initPacket->getRGID();
            if (m_debugFTDataSvcHandler) {
                printf("FTControlSvcHandler::open UUID(%s) RGID(%s)\n", m_pid->toString().c_str(), m_rgid->toString().c_str());
            }

            FTInitDataGroupReplyPacket* reply =
                    new FTInitDataGroupReplyPacket(0, initPacket->getPacketHeader()->getPacketID(), true);
            if (m_debugFTDataSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - before sending reply...\n")));
            }
            int ret = this->sendPacket(reply, 0);
            // if (m_debugFTDataSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - final %d\n"), ret));
            //}
            return ret;
        }
        case FTDataPrebindPacket::FT_DATA_PREBIND_PACKET_OP:
        {
            if (m_debugFTDataSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - PREBIND INCOMING PACKET\n")));
            }
            FTDataPrebindPacket* initPacket = static_cast<FTDataPrebindPacket*> (recvPacket);
            m_pid = initPacket->getUUID();
            m_fid = initPacket->getFID();
            m_rgid = initPacket->getRGID();
            //if (m_debugFTDataSvcHandler) {
            printf("FTDataSvcHandler::open in PREBIND MODE UUID(%s) RGID(%s)\n", m_pid->toString().c_str(), m_rgid->toString().c_str());
            //}
            FTDataPrebindReplyPacket* reply = new FTDataPrebindReplyPacket(0, initPacket->getPacketHeader()->getPacketID(), true);
            if (m_debugFTDataSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - PREBIND before sending reply...\n")));
            }
            int ret = this->sendPacket(reply, 0);
            // if (m_debugFTDataSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTDataSvcHandler::open - PREBIND final %d\n"), ret));
            //}
            return ret;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            return -1;
        }
    }
}
