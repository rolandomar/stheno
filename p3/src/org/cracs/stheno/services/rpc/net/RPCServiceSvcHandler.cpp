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
 * File:   RPCServiceSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:10 PM
 */

#include "RPCServiceSvcHandler.h"

#include <stheno/services/rpc/RPCService.h>

RPCServiceSvcHandler::RPCServiceSvcHandler(RPCService* streamServer,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new RPCServicePacketFactory(), false, false,
ec, reserve, qos, closeListener) {
    m_debugRPCServiceSvcHandler = RuntimeConfiguration::isClassDebugged("RPCServiceSvcHandler");
    m_streamServer = streamServer;
}

RPCServiceSvcHandler::~RPCServiceSvcHandler() {
}

int RPCServiceSvcHandler::handlePacket(SthenoStreamPacket* packet) {
    //ACE_Time_Value start = ACE_OS::gettimeofday();
    if (m_debugRPCServiceSvcHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceSvcHandler:handlePacket() - ID=%d UUID=%s FID=%s\n"), packet->getPacketHeader()->getPacketID(),
                this->getPID()->toString().c_str(), m_fid->toString().c_str()));
    }
    switch (packet->getPacketHeader()->getType()) {
        case RPCInvocationPacket::RPC_INVOCATION_PACKET_OP:
        {
            RPCInvocationPacket* invocationPacket = static_cast<RPCInvocationPacket*> (packet);
            if (m_debugRPCServiceSvcHandler) {
                printf("Received 2 Way Invocation!!!!!!!\n");
            }
            //TRACE: tasquito 0-1us (until next line)
            ACE_Message_Block* replyPayload = m_streamServer->handleTwoWayInvocation(invocationPacket->getOID(), invocationPacket->getPayload()->duplicate());
            //TRACE: tasquito 3-5us (after invocation)           
            
            RPCInvocationReplyPacket* replyInvocation = new RPCInvocationReplyPacket(0, invocationPacket->getPacketHeader()->getPacketID(),
                    replyPayload);//,
                    //invocationPacket->getTimestamp()
                    //);
            
            //TRACE: tasquito 4-6us (here)
            //ACE_Time_Value middle = ACE_OS::gettimeofday()-start;
            int ret = this->sendPacket(replyInvocation, 0); //TRACE: tasquito 12us (send operation)           
            //TRACE: tasquito 16-30us (here)
            delete invocationPacket;
            delete replyInvocation;
            //TRACE: tasquito 18-33us (total)            
            //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
            
            /*ULongLong deltausec;
            end.to_usec(deltausec);
            ULongLong deltausec2;
            middle.to_usec(deltausec2);
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: RPCServiceSvcHandler::handlePacket - time=%Q beforesned=%Q\n"), deltausec,deltausec2));*/
            return ret;
        }

        case RPCInvocationOneWayPacket::RPC_ONE_WAY_INVOCATION_PACKET_OP:
        {
            RPCInvocationOneWayPacket* invocationPacket = static_cast<RPCInvocationOneWayPacket*> (packet);
            if (m_debugRPCServiceSvcHandler) {
                printf("Received 1 Way Invocation!!!!!!!\n");
            }
            m_streamServer->handleOneWayInvocation(invocationPacket->getOID(), invocationPacket->getPayload()->duplicate());
            delete invocationPacket;
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

int RPCServiceSvcHandler::open(void *arg) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: RPCServiceSvcHandler::open\n")));
    int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)INFO: RPCServiceSvcHandler::open failed\n")));
        return -1;
    }
    //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
    ACE_Time_Value receiveTimeout;
    receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);


    SthenoStreamPacket* recvPacket = 0;
    receivePacket(recvPacket, &receiveTimeout);
    if (recvPacket == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceSvcHandler::open - no init packet\n")));
        return -1;
    }
    switch (recvPacket->getPacketHeader()->getType()) {

        case InitSessionPacket::INIT_SESSION_PACKET_OP:
        {//from children
            InitSessionPacket* initSessionPacket = static_cast<InitSessionPacket*> (recvPacket);
            m_pid = initSessionPacket->getUUID();
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RPCServiceSvcHandler::open - PID=%s\n"), m_pid->toString().c_str()));
            m_fid = initSessionPacket->getFID();
            m_qos = initSessionPacket->getQoS();
            return 0;
        }

        default:
        {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: RPCServiceSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
            close();
            if (recvPacket != 0) {
                delete recvPacket;
            }
            return -1;
        }
    }

}
