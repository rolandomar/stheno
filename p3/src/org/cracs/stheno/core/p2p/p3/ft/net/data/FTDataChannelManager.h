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
 * File:   FTDataChannelManager.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 11:03 AM
 */

#ifndef FTDataChannelManager_H
#define	FTDataChannelManager_H

#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>

#include <euryale/net/EventHandler.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <euryale/net/RequestGroup.h>

#include <stheno/service/exception/ServiceException.h>

#include <stheno/core/p2p/p3/ft/net/data/FTDataClientHandler.h>

class P3ReplicationGroup;

class FTDataChannelManager : public ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:    
    FTDataChannelManager(P3ReplicationGroup* rg);
    virtual ~FTDataChannelManager();

    int replicate(ACE_Message_Block* payload);

protected:
    P3ReplicationGroup* m_rg;
    virtual void fireOnClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
};

#endif	/* FTDataChannelManager_H */

