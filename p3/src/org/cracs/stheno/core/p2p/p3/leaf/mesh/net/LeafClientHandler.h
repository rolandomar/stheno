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
 * File:   LeafClientHandler.h
 * Author: rmartins
 *
 * Created on October 11, 2010, 11:25 AM
 */

#ifndef LEAFCLIENTHANDLER_H
#define	LEAFCLIENTHANDLER_H

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
//#include "ace/LSOCK_Acceptor.h"
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch_Traits.h>

#include "ace/Reactor.h"
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/JoinMeshReplyPacket.h>
#include <ace/UNIX_Addr.h>

#include <ace/Synch_Traits.h>

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <euryale/net/RequestEngine.h>

#include <stheno/core/p2p/p3/leaf/mesh/P3LeafMesh.h>

class LeafClientHandler : public PacketChannel<SthenoPacket, SthenoHeader,
//CellPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    static const int JOIN_CELL_TIMEOUT_MS = 20000;
    static const int LEAVE_CELL_TIMEOUT_MS = 20000;

    LeafClientHandler();
    LeafClientHandler(UUIDPtr& uuid, CellIDPtr& cellID,
            P3LeafMesh* mesh, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    virtual ~LeafClientHandler();

    int open(void *arg = 0);

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
        return PacketChannel<SthenoPacket, SthenoHeader, ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual ACE_Reactor *reactor(void) const;
    virtual void reactor(ACE_Reactor *reactor);

    UUIDPtr& getPID();
    CellIDPtr& getCellID();

    bool joinSiblingPeer(
            int type,
            UUIDPtr& uuid,
            CellIDPtr& cellID,
            CellIDPtr& parentCellID,
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP
            );

    bool leftSiblingPeer(UUIDPtr& uuid, CellIDPtr& cellID);

    bool rebindSiblingPeer(
            int type,
            UUIDPtr& uuid,
            CellIDPtr& cellID,
            CellIDPtr& parentCellID,
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP,
            ACE_Message_Block* mb);

    bool joinMesh(int type);
    bool rebindMesh();

    bool updateInfo(InfoUpdatePtr& updateInfoPtr);

    virtual int handlePacket(SthenoPacket* packet);
    virtual RequestEngine<SthenoPacket*>::RequestPtr* sendRequest(SthenoPacket* packet,
            ACE_Time_Value* timeout = 0);


protected:
    bool m_debugLeafClientHandler;
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;
    RequestEngine<SthenoPacket*> m_requests;
    //Cell* m_cell;
    P3LeafMesh* m_mesh;

};

typedef ACE_Strong_Bound_Ptr<LeafClientHandler, ACE_Recursive_Thread_Mutex> LeafClientHandlerPtr;

#endif	/* LEAFCLIENTHANDLER_H */

