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
 * File:   StreamSession.h
 * Author: rmartins
 *
 * Created on November 18, 2010, 12:54 PM
 */

#ifndef STREAMSESSION_H
#define	STREAMSESSION_H

#include <stheno/core/p2p/p3/leaf/stream/net/StreamSvcHandler.h>
#include <stheno/core/p2p/p3/leaf/stream/StreamListener.h>
#include <euryale/threading/Task.h>
#include <euryale/common/sleep/SleepHelper.h>

class StreamServer;

class StreamSession : public StreamListener {//: public Task {
public:
    StreamSession(StreamServer* ss, StreamSvcHandler* svcHandler);
    virtual ~StreamSession();

    int open();

    void close();

    virtual int onFrame(int frameNo);
    /*virtual int svc(void);*/
   
    StreamSvcHandler* getHandler(){
        return m_svcHandler;
    }
protected:
    StreamServer* m_ss;
    ACE_Message_Block* m_payload;
    StreamSvcHandler* m_svcHandler;
    bool m_stop;
};
#endif	/* STREAMSESSION_H */

