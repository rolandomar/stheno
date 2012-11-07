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
 * File:   FTDataChannelManager.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 11:03 AM
 */

#include "FTDataChannelManager.h"
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>

FTDataChannelManager::FTDataChannelManager(P3ReplicationGroup* rg) :
ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>(0), m_rg(rg) {
}

FTDataChannelManager::~FTDataChannelManager() {
}

void FTDataChannelManager::fireOnClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    printf("FTDataChannelManager::fireOnClose\n");
    //m_rg->onPeerClosure(channel->getPID());
}

int FTDataChannelManager::replicate(ACE_Message_Block* payload) {
    //ULongLong deltausec;
    //list<ULongLong> lr;
    //ACE_Time_Value start = ACE_OS::gettimeofday();
    //ACE_Time_Value partial = ACE_OS::gettimeofday();
    ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    RequestGroup<FTDataClientHandler, FTDataClientHandler::RequestPtr*> requestGroup(0);
    list<ChannelTypePtr> memberList;
    getMemberList(memberList);
    ace_mon.release();
    /*partial = ACE_OS::gettimeofday() - start;
    partial.to_usec(deltausec);
    lr.push_back(deltausec);*/
    
    list<FTDataClientHandler::RequestPtr*> lr;

    list<ChannelTypePtr>::iterator iter = memberList.begin();
    while (iter != memberList.end()) {
        //ACE_Time_Value t = ACE_OS::gettimeofday();
        /*FTDataClientHandler* clientHandler = static_cast<FTDataClientHandler*> ((*iter).get());
        //payload is freed upon packet deletion
        FTDataClientHandler::RequestPtr* request = clientHandler->replicate(payload->duplicate());
        if (request != 0) {
            requestGroup.addRequest(request);
        }*/
        FTDataClientHandler* clientHandler = static_cast<FTDataClientHandler*> ((*iter).get());
        FTDataClientHandler::RequestPtr* request = clientHandler->replicate(payload->duplicate());
        if (request != 0) {
            lr.push_back(request);
        }
        
        /*partial = ACE_OS::gettimeofday() - start;
        partial.to_usec(deltausec);
        lr.push_back(deltausec);*/
        iter++;
    }
       

    /*requestGroup.open();     
    list<FTDataClientHandler::RequestPtr*>* l = requestGroup.waitFuture(0);*/
    list<FTDataClientHandler::RequestPtr*>::iterator iter2 = lr.begin();
    while(iter2 != lr.end() ){
        (*(*iter2))->waitFuture(0);
        delete *iter2;
        iter2++;
    }
    //ListHelper<FTDataClientHandler::RequestPtr*>::deleteRequestList(lr);
    
    
    /*partial = ACE_OS::gettimeofday() - start;
        partial.to_usec(deltausec);
        lr.push_back(deltausec);
    
    printf("Replicate******\n");
    list<ULongLong>::iterator iter2 = lr.begin();
    while (iter2 != lr.end()) {
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: replicate latency(%Q)us\n"),
                *iter2
                ));
        iter2++;
    }*/
    
    /*if (l != 0) {
        ListHelper<FTDataClientHandler::RequestPtr*>::deleteList(l);
        return 0;
    }*/
    
    
    //requestGroup.close();
    return 0;
}