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
 * File:   ChannelRequest.h
 * Author: rmartins
 *
 * Created on September 23, 2010, 12:26 PM
 */

#ifndef CHANNELREQUEST_H
#define	CHANNELREQUEST_H

/*#include <euryale/net/Request.h>

template <class CHANNEL, class REQUEST>
class ChannelRequestEngine;

template <class CHANNEL, class REQUEST>
class ChannelRequest : public Request<REQUEST> {
public:

    ChannelRequest(CHANNEL* channel, ChannelRequestEngine<CHANNEL, REQUEST>* engine, UInt id, ACE_Time_Value* timeout);

    CHANNEL* getChannel();

protected:
    CHANNEL* m_channel;
};*/
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/event/EventFuture.h>

template <class CHANNEL, class REQUEST>
class ChannelRequestEngine;

typedef ACE_Strong_Bound_Ptr<WriteBlock, ACE_Recursive_Thread_Mutex> WriteBlockPtr;

template <class CHANNEL, class REQUEST>
class ChannelRequest : public EventFuture<REQUEST>, public EventListener {
public:

    typedef ACE_Strong_Bound_Ptr<ChannelRequest<CHANNEL, REQUEST>, ACE_Recursive_Thread_Mutex> RequestPtr;

    ChannelRequest(CHANNEL* channel, EventListener* listener, ChannelRequestEngine<CHANNEL, REQUEST>* engine, UInt id, ACE_Time_Value* timeout);
    ChannelRequest(CHANNEL* channel, ChannelRequestEngine<CHANNEL, REQUEST>* engine, UInt id, ACE_Time_Value* timeout);
    ChannelRequest(CHANNEL* channel, UInt id, ACE_Time_Value* timeout);
    ChannelRequest(CHANNEL* channel, ACE_Time_Value* timeout);
    virtual ~ChannelRequest();

    UInt getID();


    virtual int setFuture(REQUEST result);

    virtual void close();
    virtual void cancel(bool clearMap = true);
    virtual CHANNEL* getChannel();

    virtual int attachWriteBlock(WriteBlockPtr* wb);
    virtual void onEvent(Event* event, int oldState, int newState);

    void disableClearMap();


protected:
    UInt m_id;
    ChannelRequestEngine<CHANNEL, REQUEST>* m_engine;
    CHANNEL* m_channel;
    WriteBlockPtr* m_wb;
    bool m_disableClearMap;
};


#include <euryale/net/ChannelRequestEngine.h>

template <class CHANNEL, class REQUEST>
ChannelRequest<CHANNEL, REQUEST>::ChannelRequest(CHANNEL* channel, EventListener* listener,
ChannelRequestEngine<CHANNEL, REQUEST>* engine, UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(engine), m_wb(0) {
    //printf("ChannelRequest::ChannelRequest(1) %p\n",this);
    m_channel = channel;
    this->setListener(listener);
}

template <class CHANNEL, class REQUEST>
ChannelRequest<CHANNEL, REQUEST>::ChannelRequest(CHANNEL* channel,
ChannelRequestEngine<CHANNEL, REQUEST>* engine, UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(engine), m_wb(0) {
    //printf("ChannelRequest::ChannelRequest(2) %p\n",this);
    m_channel = channel;
}

template <class CHANNEL, class REQUEST>
ChannelRequest<CHANNEL, REQUEST>::ChannelRequest(CHANNEL* channel, UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(0), m_wb(0) {
    //printf("ChannelRequest::ChannelRequest(3) %p\n",this);
    m_channel = channel;
}

template <class CHANNEL, class REQUEST>
ChannelRequest<CHANNEL, REQUEST>::ChannelRequest(CHANNEL* channel, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(0), m_engine(0), m_wb(0) {
    //printf("ChannelRequest::ChannelRequest(4) %p\n",this);
    m_channel = channel;
}

template <class CHANNEL, class REQUEST>
ChannelRequest<CHANNEL, REQUEST>::~ChannelRequest() {
    //printf("ChannelRequest::~ChannelRequest(%p)\n",this);
}

template <class CHANNEL, class REQUEST>
UInt ChannelRequest<CHANNEL, REQUEST>::getID() {
    return m_id;
}

template <class CHANNEL, class REQUEST>
CHANNEL* ChannelRequest<CHANNEL, REQUEST>::getChannel() {
    return m_channel;
}

template <class CHANNEL, class REQUEST>
void ChannelRequest<CHANNEL, REQUEST>::disableClearMap() {
    m_disableClearMap = true;
}

template <class CHANNEL, class REQUEST>
void ChannelRequest<CHANNEL, REQUEST>::cancel(bool clearMap) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock));
    if (clearMap) {
        if (m_engine != 0) {
            if (!m_disableClearMap) {
                m_engine->clear(m_id);
            }
            m_engine = 0;
        }
    } else {
        m_engine = 0;
    }
    EventFuture<REQUEST>::cancel();
}

template <class CHANNEL, class REQUEST>
void ChannelRequest<CHANNEL, REQUEST>::close() {
    /*ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock));
    if (EventFuture<REQUEST>::isStateFinal()) {
        if (m_engine != 0) {
            m_engine->clear(m_id);
        }
    } else {
        cancel(true);
    }*/
    cancel(true);

}

template <class CHANNEL, class REQUEST>
int ChannelRequest<CHANNEL, REQUEST>::attachWriteBlock(WriteBlockPtr* wb) {
    if (m_wb != 0) {
        return -1;
    }
    m_wb = wb;
    (*m_wb)->setListener(this);
    return 0;
}

template <class CHANNEL, class REQUEST>
void ChannelRequest<CHANNEL, REQUEST>::onEvent(Event* event, int oldState, int newState) {
    if (event->isStateFinal()) {
        (*m_wb)->removeListener();
        delete m_wb;
        m_wb = 0;
        return;
    } else {
        if (event->getState() == Event::ES_CANCEL || event->getState() == Event::ES_ERROR || event->getState() == Event::ES_TIMEOUT) {
            cancel(true);
            return;
        }
    }
}

template <class CHANNEL, class REQUEST>
int ChannelRequest<CHANNEL, REQUEST>::setFuture(REQUEST result) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock), -1);
    m_engine = 0;
    return EventFuture<REQUEST>::setFuture(result);
}

#endif	/* CHANNELREQUEST_H */

