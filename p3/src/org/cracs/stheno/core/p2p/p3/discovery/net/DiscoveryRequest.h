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
 * File:   DiscoveryRequest.h
 * Author: rmartins
 *
 * Created on September 21, 2010, 3:53 PM
 */

#ifndef DISCOVERYREQUEST_H
#define	DISCOVERYREQUEST_H

#include <stheno/core/p2p/p3/discovery/net/P3DiscoverySvcHandler.h>
#include <stheno/core/p2p/p3/discovery/net/P3DiscoveryClientHandler.h>
#include <euryale/net/RequestGroup.h>
#include <euryale/net/StreamChannelGroup.h>

class DiscoveryRequest : public EventListener {
public:
    DiscoveryRequest(ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>::ChannelTypePtr& handlerPtr);     
    virtual ~DiscoveryRequest();

    virtual int open();
    virtual void onEvent(Event* event, int oldState, int newState);

    RequestGroup<P3DiscoveryClientHandler,
    ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr*>& getRequestGroup();

protected:
    ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>::ChannelTypePtr m_handlerPtr;
    //fast access pointer, dont delete it in the ~()
    P3DiscoverySvcHandler* m_svcHandler;
    RequestGroup<P3DiscoveryClientHandler,
    ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr*> m_requestGroup;

    void requestAction();
};

#endif	/* DISCOVERYREQUEST_H */

