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
 * File:   Request.h
 * Author: rmartins
 *
 * Created on September 23, 2010, 10:40 AM
 */

#ifndef REQUEST_H
#define	REQUEST_H

#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>


template <class REQUEST>
class RequestEngine;

template <class REQUEST>
class Request : public EventFuture<REQUEST> {
public:

    typedef ACE_Strong_Bound_Ptr<Request<REQUEST>, ACE_Recursive_Thread_Mutex> RequestPtr;

    Request(EventListener* listener, RequestEngine<REQUEST>* engine, UInt id, ACE_Time_Value* timeout);
    Request(RequestEngine<REQUEST>* engine, UInt id, ACE_Time_Value* timeout);
    Request(UInt id, ACE_Time_Value* timeout);
    Request(ACE_Time_Value* timeout);
    virtual ~Request();

    UInt getID();

    virtual void cancel(bool clearMap = true);
    
    virtual void disableClearMap();

protected:
    UInt m_id;
    bool m_disableClearMap;
    RequestEngine<REQUEST>* m_engine;
};


#include <euryale/net/RequestEngine.h>

template <class REQUEST>
Request<REQUEST>::Request(EventListener* listener, RequestEngine<REQUEST>* engine, UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(engine),m_disableClearMap(false) {
    this->setListener(listener);
}

template <class REQUEST>
Request<REQUEST>::Request(RequestEngine<REQUEST>* engine, UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(engine),m_disableClearMap(false) {
}

template <class REQUEST>
Request<REQUEST>::Request(UInt id, ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(id), m_engine(0),m_disableClearMap(false) {
}

template <class REQUEST>
Request<REQUEST>::Request(ACE_Time_Value* timeout) :
EventFuture<REQUEST>(timeout), m_id(0), m_engine(0),m_disableClearMap(false) {
}

template <class REQUEST>
void Request<REQUEST>::disableClearMap() {
    m_disableClearMap = true;
}

template <class REQUEST>
Request<REQUEST>::~Request() {
    cancel(true);
}

template <class REQUEST>
UInt Request<REQUEST>::getID() {
    return m_id;
}

template <class REQUEST>
void Request<REQUEST>::cancel(bool clearMap) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(EventFuture<REQUEST>::m_lock));
    if (clearMap) {
        if (m_engine != 0) {
            if(!m_disableClearMap){
                m_engine->clear(m_id);
            }
        }
    }
    EventFuture<REQUEST>::cancel();
}


#endif	/* REQUEST_H */

