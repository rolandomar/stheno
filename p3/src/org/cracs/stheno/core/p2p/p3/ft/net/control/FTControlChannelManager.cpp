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
 * File:   FTControlChannelManager.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 11:03 AM
 */

#include "FTControlChannelManager.h"
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>
#include <euryale/net/RequestGroup.h>

FTControlChannelManager::FTControlChannelManager(P3ReplicationGroup* rg) :
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(0), m_rg(rg) {
}

FTControlChannelManager::~FTControlChannelManager() {
}

void FTControlChannelManager::fireOnClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    printf("FTControlChannelManager::fireOnClose\n");
    //TODO:check this
    channel->setCloseListener(0);
    m_rg->onPeerClosure(channel->getPID());
    printf("FTControlChannelManager::fireOnClose after\n");
}

int FTControlChannelManager::addReplica(UUIDPtr& uuid, UUIDPtr& fid, OverlayPeerInfoPtr& pi) {
    /*RequestGroup<FTControlClientHandler, FTControlClientHandler::RequestPtr*> requestGroup(0);
    ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
    while (iter != m_map.end()) {
        printf("FTControlChannelManager::addReplica inside loop - %s  %s\n",iter->item()->getPID()->toString().c_str(),uuid->toString().c_str());
        if (!iter->item()->getPID()->isEqual(uuid)) {
            printf("FTControlChannelManager::addReplica %s  %s\n",iter->item()->getPID()->toString().c_str(),uuid->toString().c_str());
            FTControlClientHandler* clientHandler = static_cast<FTControlClientHandler*> (iter->item().get());
            FTControlClientHandler::RequestPtr* request = clientHandler->addReplica(uuid, fid,pi);
            requestGroup.setFuture(request);
        }
        iter++;
    }
    printf("FTControlChannelManager::addReplica out loop\n");
    requestGroup.open();
    printf("FTControlChannelManager::addReplica out loop 2\n");
    list<FTControlClientHandler::RequestPtr*>* l = requestGroup.waitFuture(0);
    printf("FTControlChannelManager::addReplica out loop 3\n");
    return 0;*/
    ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    RequestGroup<FTControlClientHandler, FTControlClientHandler::RequestPtr*> requestGroup(0);
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();
    list<ChannelTypePtr>::iterator iter = memberList.begin();
    //ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
    while (iter != memberList.end()) {
        printf("FTControlChannelManager::addReplica inside loop - %s  %s\n", (*iter)->getPID()->toString().c_str(), uuid->toString().c_str());
        if (!(*iter)->getPID()->isEqual(uuid)) {
            printf("FTControlChannelManager::addReplica %s  %s\n", (*iter)->getPID()->toString().c_str(), uuid->toString().c_str());
            FTControlClientHandler* clientHandler = static_cast<FTControlClientHandler*> ((*iter).get());
            FTControlClientHandler::RequestPtr* request = clientHandler->addReplica(uuid, fid, pi);
            //requestGroup.setFuture(request);
            if (request != 0) {
                requestGroup.addRequest(request);
            }
        }
        iter++;
    }
    printf("FTControlChannelManager::addReplica out loop\n");
    requestGroup.open();
    printf("FTControlChannelManager::addReplica out loop 2\n");
    list<FTControlClientHandler::RequestPtr*>* l = requestGroup.waitFuture(0);
    printf("FTControlChannelManager::addReplica out loop 3\n");
    requestGroup.close();
    return 0;
}