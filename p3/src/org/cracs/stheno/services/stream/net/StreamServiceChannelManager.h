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
 * File:   StreamServiceChannelManager.h
 * Author: rmartins
 *
 * Created on November 24, 2010, 2:55 PM
 */

#ifndef STREAMSERVICECHANNELMANAGER_H
#define	STREAMSERVICECHANNELMANAGER_H


#include <euryale/net/EventHandler.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/service/exception/ServiceException.h>

#include <stheno/services/stream/net/packet/StreamFramePacket.h>
#include <stheno/services/stream/net/StreamServiceSvcHandler.h>


class StreamServiceChannelManager: public ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    StreamServiceChannelManager(ListenerType* listener = 0);    
    
    virtual ~StreamServiceChannelManager();
    
    
    int sendFrame(StreamFramePacket* packet){
        
        //ACE_GUARD_RETURN(RECURSIVE_MUTEX, ace_mon, m_lock,-1);
        list<ChannelTypePtr> memberList;
        this->getMemberList(memberList);        
        list<ChannelTypePtr>::iterator iter = memberList.begin();
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceChannelManager::sendFrame()\n")));
        //ACE_Hash_Map_Manager<UUIDPtr, ChannelTypePtr, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();        
        while(iter != memberList.end()){
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceChannelManager::sendFrame() inside loop=%@\n"),(*iter).get()));
            StreamFramePacket* p = new StreamFramePacket(0,packet->getFrameNo(),packet->getFrameSize(), packet->getTimestamp(), packet->getPayload()->duplicate());
            //l.push_back((*iter).item()->getPID());
            //iter->item()->s
            StreamServiceSvcHandler* svcHandler = static_cast<StreamServiceSvcHandler*>((*iter).get());
            svcHandler->sendPacket(p,0);
            delete p;
            iter++;
        }        
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamServiceChannelManager::sendFrame() - end\n")));
        return 0;
    }
private:

};

#endif	/* STREAMSERVICECHANNELMANAGER_H */

