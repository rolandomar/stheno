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
 * File:   DiscoveryRequest.cpp
 * Author: rmartins
 * 
 * Created on September 21, 2010, 3:53 PM
 */

#include "DiscoveryRequest.h"

DiscoveryRequest::DiscoveryRequest(ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>::ChannelTypePtr& handlerPtr) : //P3DiscoverySvcHandler* svcHandler):
m_requestGroup(0), m_handlerPtr(handlerPtr) {
    m_svcHandler = static_cast<P3DiscoverySvcHandler*> (handlerPtr.get());
}

DiscoveryRequest::~DiscoveryRequest() {
    m_requestGroup.cancel(true);
}

int DiscoveryRequest::open() {
    m_requestGroup.setListener(this);
    return m_requestGroup.open();
}

void DiscoveryRequest::onEvent(Event* event, int oldState, int newState) {
    if (m_requestGroup.isStateFinal()) {
        m_requestGroup.removeListener();
        requestAction();
    }
}

RequestGroup<P3DiscoveryClientHandler,
ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr*>& DiscoveryRequest::getRequestGroup() {
    return m_requestGroup;
}

void DiscoveryRequest::requestAction() {

}