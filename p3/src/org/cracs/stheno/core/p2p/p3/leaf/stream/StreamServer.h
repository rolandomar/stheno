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
 * File:   StreamService.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 6:45 PM
 */

#ifndef STREAMSERVER_H
#define	STREAMSERVER_H

#include <euryale/threading/Task.h>
#include <euryale/common/sleep/SleepHelper.h>



#include <stheno/services/stream/ServiceStreamParams.h>
#include <stheno/core/p2p/p3/leaf/stream/net/StreamSvcHandler.h>
#include <stheno/core/p2p/p3/leaf/stream/net/StreamAcceptor.h>

#include <stheno/core/p2p/p3/leaf/stream/StreamListener.h>
#include <stheno/core/p2p/p3/leaf/stream/StreamSession.h>

class QoSManagerInterface;
class StreamServer : public Task {
public:
    static const int AUDIO_SENSOR = 0x01;
    static const int VIDEO_SENSOR = 0x02;
    static const int EVENT_SENSOR = 0x03;
    StreamServer(int type);
    virtual ~StreamServer();

    int open(QoSManagerInterface* qosManager);

    int createSession(StreamSvcHandler* svcHandler);

    EndpointPtr& getEndpoint();

    virtual int svc_i(void*);
    
    UInt getLowLevelFrameRate();    
    void addStreamListener(StreamListener* sl);

    QoSManagerInterface* getQoSManager();
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    list<StreamListener*> m_listeners;
    int m_type;
    int m_frameCounter;
    StreamAcceptor m_acceptor;
    bool m_stop;
    QoSManagerInterface* m_qosManager;
    //vector<ACE_Message
        
    void fireStreamListener();
    
    virtual int close (u_long flags = 0){return 0;}
};

#endif	/* STREAMSERVER_H */

