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
 * File:   ActuatorSvcHandler.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:48 PM
 */

#ifndef ActuatorSvcHandler_H
#define	ActuatorSvcHandler_H

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

#include <stheno/core/p2p/p3/leaf/actuator/net/packet/ActuatorPacketFactory.h>

class ActuatorServer;
class ActuatorSvcHandler : public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    typedef ACE_Strong_Bound_Ptr<ActuatorSvcHandler, 
        ACE_Recursive_Thread_Mutex> channel_ptr;
    
    static const int MAX_OPEN_TIMEOUT_MS = 2000;

    ActuatorSvcHandler(ActuatorServer* streamServer, //bool asyncRead, bool asyncWrite,
                   ExecutionModelPtr* ec = 0,
                   NetReservation* reserve = 0,
                   NetQoS* qos = 0,
                   CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    
    virtual ~ActuatorSvcHandler();

    virtual int open(void *arg = 0);   

    virtual int handlePacket(SthenoStreamPacket* packet);
      
    UUIDPtr& getPID();
    UUIDPtr& getFID();
        
protected:    
    ActuatorServer* m_streamServer;
    UUIDPtr m_pid;
    UUIDPtr m_fid;
    
};
#endif	/* ActuatorSvcHandler_H */

