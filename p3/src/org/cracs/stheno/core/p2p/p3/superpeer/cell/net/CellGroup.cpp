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
 * File:   CellGroup.cpp
 * Author: rmartins
 * 
 * Created on August 31, 2010, 4:33 PM
 */

#include "CellGroup.h"
#include "CellFollowerClientHandler.h"

CellGroup::CellGroup(UUIDPtr& uuid, CellIDPtr& cellID) :
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(),
m_uuid(uuid),
m_cellID(cellID) {
}

CellGroup::CellGroup(Cell* cell) :
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(cell) {

    cell->getCellID(m_cellID);
    cell->getPID(m_uuid);
}

CellGroup::~CellGroup() {
}

void CellGroup::joinPeer(int peerType,
        UUIDPtr& uuid,
        CellIDPtr& cellID,
        CellIDPtr& parentCellID,
        EndpointPtr& endpoint,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellGroup::joinPeer() - before lock\n")));
    ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellGroup::joinPeer() - after lock\n")));
    UUIDPtr srcCellID(new CellID(getCellID().get()));
    list<RequestEngine<SthenoPacket*>::RequestPtr*> requestList;
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();

    list<ChannelTypePtr>::iterator iter = memberList.begin();
    while (iter != memberList.end()) {
        CellFollowerClientHandler* svcHandler = static_cast<CellFollowerClientHandler*> ((*iter).get());
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellGroup::joinPeer() - befoer send (%s)\n"),
                svcHandler->getPID()->toString().c_str()));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellGroup::joinPeer() - befoer send (%s)\n"),
                uuid->toString().c_str()));
        if (!svcHandler->getPID()->isEqual(uuid)) {
            SthenoPacket* packet =
                    new JoinMeshPacket(getUUID(),
                    srcCellID,
                    svcHandler->getPID(),
                    srcCellID,
                    //dstCellID,
                    0,
                    peerType,
                    uuid,
                    cellID,
                    parentCellID,
                    endpoint,
                    discoverySAP,
                    meshSAP,
                    ftSAP
                    );
            RequestEngine<SthenoPacket*>::RequestPtr* r = svcHandler->sendRequest(packet);
            delete packet;
            if (r != 0) {
                requestList.push_back(r);
            }
        }
        iter++;
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellGroup::joinPeer() - after send\n")));
    list<RequestEngine<SthenoPacket*>::RequestPtr*>::iterator iterRequests = requestList.begin();
    while (iterRequests != requestList.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - Inside loop\n")));
        list<SthenoPacket*>* lr;
        lr = (*(*iterRequests))->waitFuture(0);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - Inside loop lr=%@\n"), lr));
        if (lr != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - Inside loop lr size=%d\n"), lr->size()));
            ListHelper<SthenoPacket*>::deleteList(lr);
        }
        iterRequests++;
    }
    ListHelper<RequestEngine<SthenoPacket*>::RequestPtr*>::clearList(requestList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - end loop\n")));
}

void CellGroup::rebindPeer(int peerType,
        UUIDPtr& uuid,
        CellIDPtr& cellID,
        CellIDPtr& parentCellID,
        EndpointPtr& endpoint,
        SAPInfoPtr& discoverySAP,
        SAPInfoPtr& meshSAP,
        SAPInfoPtr& ftSAP,
        ACE_Message_Block* mb) {
    ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
    UUIDPtr srcCellID(new CellID(getCellID().get()));

    //ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
    list<RequestEngine<SthenoPacket*>::RequestPtr*> requestList;
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();
    list<ChannelTypePtr>::iterator iter = memberList.begin();

    EndpointPtr followerEndpoint;

    while (iter != memberList.end()) {
        CellFollowerClientHandler* svcHandler = static_cast<CellFollowerClientHandler*> ((*iter).get());
        if (!svcHandler->getPID()->isEqual(uuid)) {
            //UUIDPtr dstCellID(new CellID(svcHandler->getCellID().get()));
            SthenoPacket* packet =
                    new RebindMeshPacket(getUUID(),
                    srcCellID,
                    svcHandler->getPID(),
                    //dstCellID,
                    srcCellID,
                    0,
                    peerType,
                    uuid,
                    cellID,
                    parentCellID,
                    endpoint,
                    followerEndpoint,
                    discoverySAP,
                    meshSAP, ftSAP,
                    mb);
            RequestEngine<SthenoPacket*>::RequestPtr* r = svcHandler->sendRequest(packet);
            delete packet;
            if (r != 0) {
                requestList.push_back(r);
            }
        }
        iter++;
    }

    list<RequestEngine<SthenoPacket*>::RequestPtr*>::iterator iterRequests = requestList.begin();
    while (iterRequests != requestList.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - rebindPeer loop, before waiting future\n")));
        list<SthenoPacket*>* lr;
        lr = (*(*iterRequests))->waitFuture(0);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - rebindPeer loop lr=%@\n"), lr));
        if (lr != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::joinPeer - rebindPeer loop lr size=%d\n"), lr->size()));
            ListHelper<SthenoPacket*>::deleteList(lr);
        }

        iterRequests++;
    }
    ListHelper<RequestEngine<SthenoPacket*>::RequestPtr*>::clearList(requestList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::rebindPeer - end loop\n")));
}

void CellGroup::leftPeer(UUIDPtr& uuid,
        CellIDPtr& cellID
        ) {
    ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer()\n")));
    if (m_map.total_size() == 0) {
        return;
    }

    UUIDPtr srcCellID(new CellID(getCellID().get()));
    list<RequestEngine<SthenoPacket*>::RequestPtr*> requestList;
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();

    list<ChannelTypePtr>::iterator iter = memberList.begin();
    while (iter != memberList.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer() - inside loop sendrequest\n")));
        CellFollowerClientHandler* svcHandler = static_cast<CellFollowerClientHandler*> ((*iter).get());
        if (!svcHandler->getPID()->isEqual(uuid)) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer() - inside loop sendrequest(2)\n")));
            SthenoPacket* packet =
                    new LeaveMeshPacket(getUUID(),
                    srcCellID,
                    svcHandler->getPID(),
                    srcCellID,
                    0,
                    uuid,
                    cellID);
            RequestEngine<SthenoPacket*>::RequestPtr* r = svcHandler->sendRequest(packet);
            delete packet;
            if (r != 0) {
                requestList.push_back(r);
            }
        }        
        iter++;
    }

    list<RequestEngine<SthenoPacket*>::RequestPtr*>::iterator iterRequests = requestList.begin();
    while (iterRequests != requestList.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer - Inside loop\n")));
        list<SthenoPacket*>* lr = 0;
        lr = (*(*iterRequests))->waitFuture(0);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer - Inside loop lr=%@\n"), lr));
        if (lr != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer - Inside loop lr size=%d\n"), lr->size()));
            ListHelper<SthenoPacket*>::deleteList(lr);
        }
        iterRequests++;
    }
    ListHelper<RequestEngine<SthenoPacket*>::RequestPtr*>::clearList(requestList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::leftPeer - end loop\n")));

}

void CellGroup::updateInfo(UUIDPtr& uuid, InfoUpdatePtr& updateInfo) {
    ACE_GUARD(RECURSIVE_MUTEX, ace_mon, m_lock);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo()\n")));
    UUIDPtr srcCellID(new CellID(getCellID().get()));
    list<RequestEngine<SthenoPacket*>::RequestPtr*> requestList;
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();
    list<ChannelTypePtr>::iterator iter = memberList.begin();

    while (iter != memberList.end()) {
        if (!(*iter)->getPID()->isEqual(uuid)) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo() - inside loop sendrequest\n")));
            CellFollowerClientHandler* svcHandler = static_cast<CellFollowerClientHandler*> ((*iter).get());
            //UUIDPtr dstCellID(new CellID(svcHandler->getCellID().get()));
            StreamSize ss;
            updateInfo->serialize(ss);
            ByteOutputStream os(ss.total_length());
            updateInfo->serialize(os);
            ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
            RDR::consolidate(mb, &os.start_);
            SthenoPacket* packet =
                    new UpdateInfoPacket(getUUID(),
                    srcCellID,
                    svcHandler->getPID(),
                    //dstCellID,
                    srcCellID,
                    0,
                    mb);
            //svcHandler->send(packet,0);
            RequestEngine<SthenoPacket*>::RequestPtr* r = svcHandler->sendRequest(packet);
            delete packet;
            if (r != 0) {
                requestList.push_back(r);
            }
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo() - inside loop sendrequest(2)\n")));
        }
        iter++;
    }

    list<RequestEngine<SthenoPacket*>::RequestPtr*>::iterator iterRequests = requestList.begin();
    while (iterRequests != requestList.end()) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo - Inside loop\n")));
        list<SthenoPacket*>* lr;
        lr = (*(*iterRequests))->waitFuture(0);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo - Inside loop lr=%@\n"), lr));
        if (lr != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo - Inside loop lr size=%d\n"), lr->size()));
            ListHelper<SthenoPacket*>::deleteList(lr);
        }

        iterRequests++;
    }
    ListHelper<RequestEngine<SthenoPacket*>::RequestPtr*>::clearList(requestList);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)CellGroup::updateInfo - end loop\n")));
}
