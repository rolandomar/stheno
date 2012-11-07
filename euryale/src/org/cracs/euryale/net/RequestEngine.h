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
 * File:   PacketEngine.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 30, 2010, 5:27 PM
 */

#ifndef PACKETENGINE_H
#define	PACKETENGINE_H

//#include <ace/Map_T.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/Types.h>
#include <euryale/event/EventFuture.h>

#include <euryale/net/Request.h>

template <class REQUEST>
class RequestEngine {
public:
    typedef ACE_Strong_Bound_Ptr<Request<REQUEST>, ACE_Recursive_Thread_Mutex> RequestPtr;

    RequestEngine() {        
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("RequestEngine::RequestEngine(%@)\n"),this));        
    }

    virtual ~RequestEngine() {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("RequestEngine::~RequestEngine(%@)\n"),this));        
        clearAll();
    }

    RequestPtr* get(UInt id) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);
        RequestPtr* request = 0;
        if (m_map.find(id, request) == 0) {
            if (request != 0) {
                request = new RequestPtr(*request);
            }            
        }
        return request;
    }

    void bind(RequestPtr* request) {
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
        m_map.bind((*request)->getID(), new RequestPtr(*request));
    }

    int setReply(UInt id, REQUEST reply) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,-1);
        RequestPtr* request = 0;
        if (m_map.find(id, request) == 0) {
            (*request)->setFuture(reply);
            clear(id);
            return 0;
        }
        return -1;
    }

    RequestPtr* unbind(UInt id) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);
        RequestPtr* request = 0;
        if (m_map.find(id, request) == 0) {
            m_map.unbind(id);
            return request;
        }
        return 0;
    }

    void clear(UInt id) {
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
        RequestPtr* request = unbind(id);
        if (request != 0) {
            delete request;
        }
    }
    
    void clearAll(){
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);        
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("RequestEngine::clearAll(%@)\n"),this));        
        typename ACE_Hash_Map_Manager<UInt, RequestPtr*, ACE_SYNCH_RW_MUTEX>::iterator iter = m_map.begin();
        while(iter != m_map.end()){
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("RequestEngine::clearAll(%@) with request...\n"),this));        
            RequestPtr* item = iter->item();
            if(!(*item).null()){
                (*item)->disableClearMap();
                (*item)->cancel(false);
            }            
            delete item;
            iter++;
        };            
        m_map.unbind_all();
    }
    
protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    ACE_Hash_Map_Manager<UInt, RequestPtr*, ACE_SYNCH_RW_MUTEX> m_map;

};

#endif	/* PACKETENGINE_H */

