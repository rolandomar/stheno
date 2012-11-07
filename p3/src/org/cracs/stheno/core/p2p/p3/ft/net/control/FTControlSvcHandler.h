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
 * File:   FTControlSvcHandler.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:28 AM
 */

#ifndef FTCONTROLSVCHANDLER_H
#define	FTCONTROLSVCHANDLER_H

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
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>

#include <stheno/core/p2p/p3/ft/net/control/packet/FTControlPacketFactory.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

class P3FaultTolerance;

class FTControlSvcHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ACE_Strong_Bound_Ptr<FTControlSvcHandler, 
        ACE_Recursive_Thread_Mutex> channel_ptr;
    
    static const int MAX_OPEN_TIMEOUT_MS = 2000;

    FTControlSvcHandler(P3FaultTolerance* ft, //bool asyncRead, bool asyncWrite,
                   ExecutionModelPtr* ec = 0,
                   NetReservation* reserve = 0,
                   NetQoS* qos = 0,
                   CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0) :
    PacketChannel<SthenoStreamPacket, SthenoStreamHeader, 
    ACE_SOCK_Stream, ACE_MT_SYNCH>(new FTControlPacketFactory(),false, false, 
    ec, reserve, qos, closeListener)/*,m_requests(0)*/ {
        m_debugFTControlSvcHandler = RuntimeConfiguration::isClassDebugged("FTControlSvcHandler");
        m_ft = ft;
        //m_requests = new RequestEngine<SthenoStreamPacket*>();
    }
    
    virtual ~FTControlSvcHandler();

    virtual int open(void *arg = 0);

    /*virtual int close(u_long flags = 0) {
        int ret = PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //DiscoveryPacketFactory,
            ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
        if (ret == 0) {
            if(m_requests != 0){
                delete m_requests;
                m_requests = 0;
            }
        }
    }*/
        
    virtual int handlePacket(SthenoStreamPacket* packet);
       
    
    UUIDPtr& getPID() {
        return m_pid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }
    
    UUIDPtr& getRGID() {
        return m_rgid;
    }
    
             
    
    void setReplicationGroup(ReplicationGroupPtr& replicationGroup);
    /*RequestEngine<SthenoStreamPacket*>::RequestPtr* sendRequest(SthenoStreamPacket* packet,
                                                          ACE_Time_Value* timeout = 0) {
        ACE_Message_Block* mb = serializePacket(packet);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3DiscoverySvcHandler:sendRequest() - ID=%d UUID=%s CELLID=%s\n"), packet->getPacketHeader()->getPacketID(),
                  this->getPID()->toString().c_str(), m_cellID->toString().c_str()));
        UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestEngine<SthenoStreamPacket*>::RequestPtr* request =
            new RequestEngine<SthenoStreamPacket*>::RequestPtr(new Request<SthenoStreamPacket*>(m_requests, id, timeout));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - before bind PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        m_requests->bind(request);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO:: P3DiscoverySvcHandler::sendRequest() - after bind PeerID=%s ID=%d\n"),
            this->getPID()->toString().c_str(),
            packet->getPacketHeader()->getPacketID()));
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
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
    }*/


protected:    
    bool m_debugFTControlSvcHandler;
    ReplicationGroupPtr m_replicationGroup;
    P3FaultTolerance* m_ft;
    
    UUIDPtr m_pid;    
    UUIDPtr m_fid;
    UUIDPtr m_rgid;
    //we need this
    //RequestEngine<SthenoStreamPacket*>* m_requests;


};



#endif	/* FTCONTROLSVCHANDLER_H */

