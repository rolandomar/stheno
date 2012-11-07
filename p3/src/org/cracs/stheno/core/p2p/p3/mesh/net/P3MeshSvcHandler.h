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
 * File:   P3MeshSvcHandler.h
 * Author: rmartins
 *
 * Created on September 30, 2010, 6:25 PM
 */

#ifndef P3MESHSVCHANDLER_H
#define	P3MESHSVCHANDLER_H


#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Reactor.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Stream.h"
#include "ace/LSOCK_Connector.h"
#include "ace/Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/net/PacketChannel.h>
#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/net/packet/InitSessionPacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshPacketFactory.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "ace/Reactor.h"
#include "ace/INET_Addr.h"
#include "ace/LSOCK_Stream.h"
#include "ace/LSOCK_Connector.h"
#include "ace/Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/net/PacketChannel.h>
#include "ace/Auto_Ptr.h"
#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include "ace/Reactor.h"
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/net/packet/InitSessionPacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshPacketFactory.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>



class P3Mesh;
class P3MeshSvcHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ACE_Strong_Bound_Ptr<P3MeshSvcHandler, 
        ACE_Recursive_Thread_Mutex> channel_ptr;
    
    static const int MAX_OPEN_TIMEOUT_MS = 2000;

    P3MeshSvcHandler(P3Mesh* mesh, //bool asyncRead, bool asyncWrite,
                   ExecutionModelPtr* ec = 0,
                   NetReservation* reserve = 0,
                   NetQoS* qos = 0,
                   CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0) :
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
    ACE_SOCK_Stream, ACE_MT_SYNCH>(new P3MeshPacketFactory(),false, false,
    ec, reserve, qos, closeListener)/*,m_requests(0)*/ {
        m_mesh = mesh;
        //m_requests = new RequestEngine<SthenoStreamPacket*>();
    }
    
    virtual ~P3MeshSvcHandler();

    virtual int open(void *arg = 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CellSvcHandler::open\n")));
        int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CellSvcHandler::open failed\n")));
            return -1;
        }
        //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
        ACE_Time_Value receiveTimeout;
        receiveTimeout.msec(MAX_OPEN_TIMEOUT_MS);

        SthenoStreamPacket* recvPacket = 0;
        receivePacket(recvPacket, &receiveTimeout);
        if (recvPacket == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - no init packet\n")));
            return -1;
        }
        switch (recvPacket->getPacketHeader()->getType()) {
            case InitSessionPacket::INIT_SESSION_PACKET_OP:
            {//from children
                InitSessionPacket* initSessionPacket = static_cast<InitSessionPacket*> (recvPacket);
                m_pid = initSessionPacket->getUUID();
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - PID=%s\n"),m_pid->toString().c_str()));
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open - FID=%s\n"),initSessionPacket->getFID()->toString().c_str()));
                UUID* uuid = new UUID(*initSessionPacket->getFID().get());
                
                CellIDPtr cellID(new CellID(uuid));
                m_cellID = cellID;
                m_qosE = initSessionPacket->getQoS();
                /*if (m_cell->handleJoinMesh(this, joinMeshPacket) != 0) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellSvcHandler::open() - processJoinCell failde\n")));
                    return -1;
                }*/                
                return 0;
            }
           
            default:
            {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
                close();
                return -1;
            }
        }
    }

    /*virtual int close(u_long flags = 0) {
        int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
        if (ret == 0) {
            if(m_requests != 0){
                delete m_requests;
                m_requests = 0;
            }
        }
    }*/
    virtual int close(u_long flags = 0) {
        {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        if (!m_close) {
            /*if (m_requests != 0) {
                delete m_requests;
                m_requests = 0;
            }*/
            m_requests.clearAll();
        }
        }
        return PacketChannel<SthenoStreamPacket, SthenoStreamHeader,ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual int handlePacket(SthenoStreamPacket* packet);
    
    UUIDPtr& getPID() {
        return m_pid;
    }

    CellIDPtr& getCellID() {
        return m_cellID;
    }
    
    QoSEndpoint& getQoSEndpoint(){
        return m_qosE;
    }

    /*bool isChildren() {
        return !((*m_cell->getCellID()).isEqual(*m_cellID));
    }*/

    RequestEngine<SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
                                                          ACE_Time_Value* timeout = 0) {
        ACE_Message_Block* mb = serializePacket(packet);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler:sendRequest() - ID=%d UUID=%s CELLID=%s\n"), packet->getPacketHeader()->getPacketID(),
                  this->getPID()->toString().c_str(), m_cellID->toString().c_str()));
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestEngine<SthenoStreamPacket*>::RequestPtr* request =
            new RequestEngine<SthenoStreamPacket*>::RequestPtr(new Request<SthenoStreamPacket*>(&m_requests, id, timeout));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - before bind PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        m_requests.bind(request);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - after bind PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - sent PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR:: P3DiscoverySvcHandler::sendRequest() - send PeerID=%s ID(%d) failed!\n"),
                    this->getPID()->toString().c_str(),
                    id));
            delete request;
            return 0;
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - END PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        return request;
    }


protected:
    //Cell* m_cell;
    P3Mesh* m_mesh;
    UUIDPtr m_pid;
    CellIDPtr m_cellID;
    QoSEndpoint m_qosE;
    //we need this
    RequestEngine<SthenoStreamPacket*> m_requests;


};

#endif	/* P3MESHSVCHANDLER_H */

