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
 * File:   CellFollowerSvcHandler.h
 * Author: rmartins
 *
 * Created on December 10, 2010, 11:01 AM
 */

#ifndef CELLFOLLOWERSVCHANDLER_H
#define	CELLFOLLOWERSVCHANDLER_H


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

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketFactory.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <euryale/net/RequestEngine.h>

class CellFollowerSvcHandler : public PacketChannel<SthenoPacket, SthenoHeader,
//CellPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    CellFollowerSvcHandler();

    CellFollowerSvcHandler(Cell* cell, bool asyncRead, bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);

    virtual ~CellFollowerSvcHandler();

    int open(void *arg = 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CellFollowerSvcHandler::open\n")));
        int ret = PacketChannel<SthenoPacket, SthenoHeader, //P3MeshPacketFactory,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CellFollowerSvcHandler::open failed\n")));
            return -1;
        }
        //ACE_Time_Value receiveTimeout(0, MAX_OPEN_TIMEOUT_MS * 1000);
        ACE_Time_Value receiveTimeout;
        receiveTimeout.msec(2000);

        SthenoPacket* recvPacket = 0;
        receivePacket(recvPacket, &receiveTimeout);
        if (recvPacket == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellFollowerSvcHandler::open - no init packet\n")));
            return -1;
        }
        switch (recvPacket->getPacketHeader()->getType()) {
            case CellInitSession::CELL_INIT_SESSION_PACKET_OP:
            {
                CellInitSession* initSessionPacket = static_cast<CellInitSession*> (recvPacket);
                m_uuid = initSessionPacket->getPacketHeader()->getDstPID();
                m_fid = initSessionPacket->getPacketHeader()->getDstFID();
                UUID* fid = new UUID(*m_fid.get());
                CellIDPtr cellID(new CellID(fid));
                m_cellID = cellID;
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellFollowerSvcHandler::open - FID=%s\n"), m_fid->toString().c_str()));
                return 0;
            }

            default:
            {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerSvcHandler::open() - processJoinCell packet not known, calling close()\n")));
                close();
                return -1;
            }
        }
    }

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

    virtual int handlePacket(SthenoPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {
            case JOIN_MESH_PACKET:
            {
                JoinMeshPacket* joinMeshPacket = static_cast<JoinMeshPacket*> (packet);
                return m_cell->handleCoordinatorJoinMesh(this, joinMeshPacket);
            }
            case LEAVE_MESH_PACKET:
            {
                LeaveMeshPacket* leaveMeshPacket = static_cast<LeaveMeshPacket*> (packet);
                return m_cell->handleCoordinatorLeaveMesh(this, leaveMeshPacket);
            }

            case REBIND_MESH_PACKET:
            {
                RebindMeshPacket* rebindMeshPacket = static_cast<RebindMeshPacket*> (packet);
                return m_cell->handleCoordinatorRebindMesh(this, rebindMeshPacket);
            }

            case UPDATEINFO_MESH_PACKET:
            {
                UpdateInfoPacket* updateInfoPacket = static_cast<UpdateInfoPacket*> (packet);
                return m_cell->handleCoordinatorUpdateInfo(this, updateInfoPacket);
            }

            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerSvcHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        return 0;
    }

    UUIDPtr& getPID() {
        return m_uuid;
    }
protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    CellIDPtr m_cellID;
    bool m_debugClientHandler;
    RequestEngine<SthenoPacket*> m_requests;
    Cell* m_cell;

};


#endif	/* CELLFOLLOWERSVCHANDLER_H */

