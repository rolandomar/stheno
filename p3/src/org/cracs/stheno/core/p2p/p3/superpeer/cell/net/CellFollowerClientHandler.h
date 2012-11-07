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
 * File:   CellFollowerClientHandler.h
 * Author: rmartins
 *
 * Created on December 10, 2010, 11:01 AM
 */

#ifndef CELLFOLLOWERCLIENTHANDLER_H
#define	CELLFOLLOWERCLIENTHANDLER_H
//only for updates to cell


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

class CellFollowerClientHandler : public PacketChannel<SthenoPacket, SthenoHeader,
//CellPacketFactory, 
ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:

    CellFollowerClientHandler();

    CellFollowerClientHandler(
            UUIDPtr& uuid,
            UUIDPtr& fid,
            Cell* cell,
            bool asyncRead,
            bool asyncWrite,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener = 0);
    ~CellFollowerClientHandler();

    int open(void *arg = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, -1);
        UUIDPtr uuid;
        m_cell->getPID(uuid);
        UUIDPtr fid;
        m_cell->getFID(fid);
        //CellInitSession* initPacket = new CellInitSession(m_cell->getFID(), m_cell->getFID(), m_uuid, m_fid, 0);
        CellInitSession* initPacket = new CellInitSession(uuid,fid, m_uuid, m_fid, 0);
        ACE_Time_Value* timeout = 0;
        int ret = sendPacket((SthenoPacket*&) initPacket, timeout);
        delete initPacket;
        if (ret == -1) {
            return -1;
        }
        ret = PacketChannel<SthenoPacket, SthenoHeader,
                ACE_SOCK_Stream, ACE_MT_SYNCH>::open(arg);
        return ret;
    }

    int close(u_long flags = 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerClientHandler:sendRequest() - close() %@\n"), this));
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, -1);
            if (!m_close) {
                m_requests.clearAll();

            }
        }
        return PacketChannel<SthenoPacket, SthenoHeader, ACE_SOCK_Stream, ACE_MT_SYNCH>::close(flags);
    }

    virtual int handlePacket(SthenoPacket* packet) {
        switch (packet->getPacketHeader()->getType()) {
            case JOIN_CELL_REPLY_PACKET:
            {
                JoinCellReplyPacket* replyPacket = static_cast<JoinCellReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellFollowerClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getRequestID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }

            case UPDATEINFO_MESH_REPLY_PACKET:
            {
                UpdateInfoReplyPacket* replyPacket = static_cast<UpdateInfoReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellFollowerClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getReplyPacketID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }
                /////////////////////////////////////////////////////
            case JOIN_MESH_REPLY_PACKET:
            {
                JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellFollowerClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getRequestID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }

            case REBIND_MESH_REPLY_PACKET:
            {
                RebindMeshReplyPacket* replyPacket = static_cast<RebindMeshReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellFollowerClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getRequestID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }

            case LEAVE_MESH_REPLY_PACKET:
            {
                JoinMeshReplyPacket* replyPacket = static_cast<JoinMeshReplyPacket*> (packet);
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellFollowerClientHandler::handlePacket() - ID(%d)\n"), replyPacket->getRequestID()));
                if (m_requests.setReply(replyPacket->getRequestID(), packet) == -1) {
                    delete packet;
                    return -1;
                }
                return 0;
            }

            default:
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
                delete packet;
                return 0;
        }
        //}
        return 0;
    }

    virtual RequestEngine<SthenoPacket*>::RequestPtr* sendRequest(SthenoPacket* packet,
            ACE_Time_Value* timeout = 0) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, SuperType::m_lock, 0);
        if (this->m_close) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerClientHandler:sendRequest() - %@ already closed \n"), this));
            return 0;
        }
        ACE_Message_Block* mb = serializePacket(packet);
        UInt id = ((SthenoHeader*) (packet->getPacketHeader()))->getPacketID();
        RequestEngine<SthenoPacket*>::RequestPtr* request =
                new RequestEngine<SthenoPacket*>::RequestPtr(new Request<SthenoPacket*>(&m_requests, id, timeout));
        m_requests.bind(request);
        int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
        ACE_Message_Block::release(mb);
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellFollowerClientHandler:sendRequest() - ID(%d)\n"), id));
            delete request;
            return 0;
        }
        return request;
    }

    UUIDPtr& getPID() {
        return m_uuid;
    }
protected:
    bool m_debugClientHandler;
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    //RequestEngine<SthenoPacket*>* m_requests;
    RequestEngine<SthenoPacket*> m_requests;
    Cell* m_cell;

};


#endif	/* CELLFOLLOWERCLIENTHANDLER_H */

