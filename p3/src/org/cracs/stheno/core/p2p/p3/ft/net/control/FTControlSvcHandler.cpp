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
 * File:   FTControlSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:28 AM
 */

#include "FTControlSvcHandler.h"
#include "stheno/core/p2p/p3/ft/P3FaultTolerance.h"
#include <stheno/core/p2p/Overlay.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

FTControlSvcHandler::~FTControlSvcHandler() {
    close();
}

int FTControlSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    if (m_debugFTControlSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_fid->toString().c_str()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case FTAddReplicaPacket::ADD_REPLICA_PACKET_OP:
        {
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler:handlePacket() - ADD_REPLICA_PACKET_OP ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
            }
            P3ReplicationGroup* rg = static_cast<P3ReplicationGroup*> (m_replicationGroup.get());
            return rg->handleAddReplicaPacket(this, static_cast<FTAddReplicaPacket*> (packet));
        }
        case FTRemoveReplicaPacket::REMOVE_REPLICA_PACKET_OP:
        {
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler:handlePacket() - REMOVE_REPLICA_PACKET_OP ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                        this->getPID()->toString().c_str(), m_fid->toString().c_str()));
            }
            P3ReplicationGroup* rg = static_cast<P3ReplicationGroup*> (m_replicationGroup.get());
            return rg->handleRemoveReplicaPacket(this, static_cast<FTRemoveReplicaPacket*> (packet));
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlSvcHandler::handlePacket() - unknow packet(%d)\n"), packet->getPacketHeader()->getType()));
            delete packet;
            return -1;
        }
    }
    return -1;
}

int FTControlSvcHandler::open(void *arg) {
    if (m_debugFTControlSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: FTControlSvcHandler::open\n")));
    }
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: FTControlSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

    SthenoStreamPacket* recvPacket = 0;
    if (m_debugFTControlSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - wait for packet..\n")));
    }
    receivePacket(recvPacket, &receiveTimeout);
    if (m_debugFTControlSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - after wait for packet..\n")));
    }
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: FTControlSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {
        case FTInitReplicaPacket::INIT_REPLICA_PACKET_OP:
        {
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - INCOMING PACKET\n")));
            }
            FTInitReplicaPacket* initPacket = static_cast<FTInitReplicaPacket*> (recvPacket);
            m_pid = initPacket->getUUID();
            m_fid = initPacket->getFID();
            m_rgid = initPacket->getRGID();
            //list<OverlayPeerInfoPtr>* peers = new list<OverlayPeerInfoPtr > (initPacket->getReplicationGroupMembers());            
            // ========================   OLD  =================================
            //Bootstraping a replica (and replica group in this runtime)
            //Allocate QoS for service and group
            /*QoSManagerInterface* qosManager = m_ft->getQoSManagerForReplicationGroup(
                    initPacket->getSID(),
                    initPacket->getRGID(),
                    initPacket->getServiceParams());
            
            //create the replication group (and replica)
            m_ft->createReplicationGroup(
                    initPacket->getPrimary(),
                    initPacket->getReplicationGroupMembers(),
                    initPacket->getRGID(),
                    initPacket->getSID(),
                    //initPacket->getIID(),
                    initPacket->getServiceParams(),
                    qosManager);*/
            // ========================   OLD  =================================
            m_ft->getOverlay()->getRuntime()->createReplicationGroup(initPacket->getPrimary(),
                    initPacket->getReplicationGroupMembers(),
                    initPacket->getRGID(),
                    initPacket->getSID(),
                    //initPacket->getIID(),
                    initPacket->getServiceParams());

            if (m_debugFTControlSvcHandler) {
                printf("FTControlSvcHandler::open UUID(%s) RGID(%s)\n", m_pid->toString().c_str(), m_rgid->toString().c_str());
            }
            m_ft->getReplicationGroup(m_rgid, m_replicationGroup);
            FTInitReplicaReplyPacket* reply = new FTInitReplicaReplyPacket(0, initPacket->getPacketHeader()->getPacketID(), m_replicationGroup->getFTDataSAPInfo());
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - before sending reply...\n")));
            }
            int ret = this->sendPacket(reply, 0);
            //if (m_debugFTControlSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - final %d\n"), ret));
            //}
            return ret;
        }
        case FTPrebindPacket::PREBIND_PACKET_OP:
        {
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - PREBIND INCOMING PACKET\n")));
            }
            FTPrebindPacket* initPacket = static_cast<FTPrebindPacket*> (recvPacket);
            m_pid = initPacket->getUUID();
            m_fid = initPacket->getFID();
            m_rgid = initPacket->getRGID();
            if (m_debugFTControlSvcHandler) {
                printf("FTControlSvcHandler::open in PREBIND MODE UUID(%s) RGID(%s)\n", m_pid->toString().c_str(), m_rgid->toString().c_str());
            }
            FTPrebindReplyPacket* reply = new FTPrebindReplyPacket(0, initPacket->getPacketHeader()->getPacketID(), true);
            if (m_debugFTControlSvcHandler) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - PREBIND before sending reply...\n")));
            }
            int ret = this->sendPacket(reply, 0);
            //if (m_debugFTControlSvcHandler) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: FTControlSvcHandler::open - PREBIND final %d\n"), ret));
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

void FTControlSvcHandler::setReplicationGroup(ReplicationGroupPtr& replicationGroup) {
    m_replicationGroup = replicationGroup;
}

