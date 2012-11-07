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
 * File:   TPCPerConnectionEngine.h
 * Author: rmartins
 *
 * Created on February 12, 2011, 4:44 PM
 */

#ifndef TPCPERCONNECTIONENGINE_H
#define	TPCPERCONNECTIONENGINE_H

#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Impl.h>
#include <ace/TP_Reactor.h>

#include <euryale/net/EventHandler.h>
#include <euryale/threading/ExecutionModel.h>
#include <euryale/threading/exception/ExecutionModelException.h>

#include <ace/Synch_Traits.h>
#include <ace/Refcounted_Auto_Ptr.h>



class TPCTask;

class ThreadPerConnectionEngine : public ExecutionModel, EventHandlerSpecialization {
public:
    ThreadPerConnectionEngine(ExecutionModelResources* resources) throw(ExecutionModelException&);
    ThreadPerConnectionEngine() throw(ExecutionModelException&);

    virtual ~ThreadPerConnectionEngine();

    ACE_Event_Handler* getHandler();

    //Doesnt allow joins
    virtual void join(Event* event = 0, ACE_Time_Value* timeout = 0) throw(ExecutionModelException&);    
    virtual void bind(EventHandler* channel) throw(ExecutionModelException&);
    virtual void unbind(EventHandler* channel) throw(ExecutionModelException&);
    virtual void open(CPUReservation* reserve,CPUQoS* qos,TPCTask* task) throw(ExecutionModelException&);

    ACE_Reactor* allocateReactorImpl(void);

    virtual void shutdown() throw(ExecutionModelException&);
    void shutdown_i(bool reactorEvent) throw(ExecutionModelException&);

    bool isShutdown();

    virtual int handleCloseImpl(EventHandler* handler, ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_debugThreadPerConnection;
    EventHandler* m_handler;
    ACE_thread_t m_owner;
    //TPCTask* m_tpc;
    //ACE_Thread_Manager m_tpcThrManager;
    bool m_close;
    bool m_hasThread;

    //void open_i() throw(ExecutionModelException&);

    virtual void join_i(Event* event, ACE_Time_Value* timeout = 0) throw(ExecutionModelException&);
};

typedef ACE_Strong_Bound_Ptr<ThreadPerConnectionEngine, ACE_SYNCH_MUTEX> ThreadPerConnectionEnginePtr;


#endif	/* TPCPERCONNECTIONENGINE_H */

