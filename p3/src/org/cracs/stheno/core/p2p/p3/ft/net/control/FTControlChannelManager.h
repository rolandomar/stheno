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
 * File:   FTControlChannelManager.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 11:03 AM
 */

#ifndef FTCONTROLCHANNELMANAGER_H
#define	FTCONTROLCHANNELMANAGER_H

#include <euryale/net/EventHandler.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/service/exception/ServiceException.h>

#include <stheno/core/p2p/p3/ft/net/control/FTControlClientHandler.h>

class P3ReplicationGroup;
class FTControlChannelManager: public ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    FTControlChannelManager(P3ReplicationGroup* rg);    
    
    virtual ~FTControlChannelManager();
    
    int addReplica(UUIDPtr& uuid,UUIDPtr& fid,OverlayPeerInfoPtr& pi);
    
protected:
    P3ReplicationGroup* m_rg;
    
    virtual void fireOnClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);

};

#endif	/* FTCONTROLCHANNELMANAGER_H */

