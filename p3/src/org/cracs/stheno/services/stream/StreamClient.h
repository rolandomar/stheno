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
 * File:   StreamClient.h
 * Author: rmartins
 *
 * Created on November 22, 2010, 11:05 PM
 */

#ifndef STREAMCLIENT_H
#define	STREAMCLIENT_H

#include <stheno/service/ServiceClient.h>
#include <stheno/services/stream/net/StreamServiceClientHandler.h>
#include <euryale/net/AbstractStreamChannel.h>
class StreamClient:public ServiceClient,public CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>,public FrameListener {
public:
    StreamClient(Stheno* runtime);
    
    virtual ~StreamClient();
    
    virtual int open(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp,UInt lastFrameNo = 0);
    //client shutdown/close
    virtual void close();
    //has to have the same SID of the service it belongs
    virtual const UUIDPtr& getSID();
    
    virtual void onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel);
        
    virtual void onFrame(StreamFramePacket* frame);

    int getCounter(){
      return m_counter;
    }
    
    ACE_Time_Value& getTotalLatency(){
      return m_totalLatency;
    }
    ACE_Time_Value& getRebindTotalTime() {
    return this->m_rebindTotalTime;
}
    
private:
    bool m_closed;
    UInt m_lastFrameNo;
    UInt m_frameCount;
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    StreamServiceClientHandler* m_client;
    ClientParamsPtr m_cp;
    ACE_Time_Value m_rebindTotalTime;
    ACE_Time_Value m_rebindStart;
    int m_counter;
    ACE_Time_Value m_totalLatency;
};

#endif	/* STREAMCLIENT_H */

