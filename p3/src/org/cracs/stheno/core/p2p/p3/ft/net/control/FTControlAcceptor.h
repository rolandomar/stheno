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
 * File:   FTControlAcceptor.h
 * Author: rmartins
 *
 * Created on November 25, 2010, 9:32 AM
 */

#ifndef FTCONTROLACCEPTOR_H
#define	FTCONTROLACCEPTOR_H

#include <euryale/net/EventHandler.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/service/exception/ServiceException.h>
#include <stheno/core/p2p/p3/ft/net/control/FTControlChannelManager.h>



class P3FaultTolerance;
//class P3ReplicationGroup;
class FTControlAcceptor : public EventHandler, public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    FTControlAcceptor(P3FaultTolerance* discovery);                
    virtual ~FTControlAcceptor();       
    
    virtual int close();
    
    int open(EndpointPtr& endpointPtr) throw (ExecutionModelException&);
    
    // Get this handler's I/O handle.
    virtual ACE_HANDLE get_handle(void) const;
    // Called when a connection is ready to accept.
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);
    // Called when this handler is removed from the ACE_Reactor.
    virtual int handle_close(ACE_HANDLE handle,
            ACE_Reactor_Mask close_mask);
    //handle close of connection
    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);    
    int getListenAddr(ACE_INET_Addr& addr);
    
    EndpointPtr& getEndpoint();
    
    /*FTControlChannelManager& getChannelManager(){
        return m_scvHandlers;
    }*/
    
protected:       
    ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* m_tpcPrt;
    //FTControlChannelManager m_scvHandlers;
    EndpointPtr m_endpointPtr;
    ACE_SOCK_Acceptor m_acceptor;    
    P3FaultTolerance* m_ft;        
};


#endif	/* FTCONTROLACCEPTOR_H */

