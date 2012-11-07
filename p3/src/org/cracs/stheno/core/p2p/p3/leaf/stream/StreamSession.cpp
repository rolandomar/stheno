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
 * File:   StreamSession.cpp
 * Author: rmartins
 * 
 * Created on November 18, 2010, 12:54 PM
 */

#include "StreamSession.h"
#include "StreamServer.h"
#include "StreamListener.h"

StreamSession::StreamSession(StreamServer* ss, StreamSvcHandler* svcHandler) :
m_ss(ss), m_svcHandler(svcHandler) {
    m_stop = false;
    size_t size = svcHandler->getFrameSize();
    m_payload = new ACE_Message_Block(size);
    m_payload->wr_ptr(size);
}

StreamSession::~StreamSession() {    
    if (m_svcHandler != 0) {
        m_svcHandler->close();
        delete m_svcHandler;
    }
    delete m_payload;
}

int StreamSession::open() {    
    m_stop = false;
    m_ss->addStreamListener(this);    
    //return Task::activate();
    return 0;
}

void StreamSession::close() {
    m_stop = true;
    m_svcHandler->close();
}

int StreamSession::onFrame(int frameNo) {
    //printf("Sending frame...this=%p\n", this);
    if (m_svcHandler->sendFrame(frameNo,m_payload->duplicate()) == -1) {
        //printf("Sending frame... error\n");
        m_svcHandler->close();
        //printf("Sending frame... error after close\n");
        return -1;
    }
    return 0;
}

/*int StreamSession::svc(void) {
        float sleepTime = ((float)1/(float)m_svcHandler->getFrameRate())*1000000;        
        while(!m_stop){
            printf("Sending frame...this=%p\n",this);
            if(m_svcHandler->sendFrame(m_payload->duplicate()) == -1){
                printf("Sending frame... error\n");
                m_svcHandler->close();
                printf("Sending frame... error after close\n");
                break;
            }
            SleepHelper::sleep(0,sleepTime);
            //hack
            //SleepHelper::sleep(2,0);
        }
        printf("Ending stream session! this=%p\n",this);
        //delete m_svcHandler;
        m_svcHandler = 0;
        delete this;
        return 0;
    }*/