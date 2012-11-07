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
 * File:   StreamChannelAcceptor.h
 * Author: rmartins
 *
 * Created on October 27, 2010, 10:42 AM
 */

#ifndef STREAMCHANNELACCEPTOR_H
#define	STREAMCHANNELACCEPTOR_H

#include <euryale/net/EventHandler.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/service/exception/ServiceException.h>

#include <euryale/net/StreamChannelGroup.h>


class StreamChannelAcceptor;
class StreamChannel;

class IncomingStreamClientHandler {
public:
    virtual int handleNewClient(StreamChannelAcceptor* acceptor) = 0;
    virtual int handleCloseClient(StreamChannelAcceptor* acceptor, StreamChannel* channel) = 0;
    //virtual ChannelManager* getChannelManager() = 0;
};

class StreamChannelAcceptor : public EventHandler, public ChannelManagerListener<ACE_SOCK_Stream, ACE_MT_SYNCH>  {//, public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    StreamChannelAcceptor(IncomingStreamClientHandler* clientHandler);
    virtual ~StreamChannelAcceptor();

    virtual int close();

    int open(EndpointPtr& endpointPtr) throw (ExecutionModelException&);

    virtual ACE_HANDLE get_handle(void) const;

    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handle_close(ACE_HANDLE handle,
            ACE_Reactor_Mask close_mask);

    //virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);

    int getListenAddr(ACE_INET_Addr& addr);

    EndpointPtr& getEndpoint();

    virtual ACE_SOCK_Acceptor& getAcceptorSock();

    ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>& getChannelManager() {
        return m_scvHandlers;
    }

    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    virtual void onAdd(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
    
protected:
    ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* m_tpcPrt;
    IncomingStreamClientHandler* m_clientHandler;
    ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> m_scvHandlers;
    EndpointPtr m_endpointPtr;
    ACE_SOCK_Acceptor m_acceptor;
};


#endif	/* STREAMCHANNELACCEPTOR_H */

