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
 * File:   ThreadPerConnection.h
 * Author: rmartins
 *
 * Created on December 9, 2009, 11:04 PM
 */

#ifndef _THREADPERCONNECTION_H
#define	_THREADPERCONNECTION_H
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Impl.h>
#include <ace/TP_Reactor.h>

#include <euryale/net/EventHandler.h>
#include <euryale/threading/ExecutionModel.h>
#include <euryale/threading/exception/ExecutionModelException.h>

#include <euryale/threading/tpc/ThreadPerConnectionEngine.h>

class TPCTask;

class ThreadPerConnection : public ExecutionModel, EventHandlerSpecialization {
public:
    ThreadPerConnection(ExecutionModelResources* resources) throw(ExecutionModelException&);
    ThreadPerConnection() throw(ExecutionModelException&);

    virtual ~ThreadPerConnection();

    ACE_Event_Handler* getHandler();

    //Doesnt allow joins
    virtual void join(Event* event = 0, ACE_Time_Value* timeout = 0) throw(ExecutionModelException&);    
    virtual void bind(EventHandler* channel) throw(ExecutionModelException&);
    virtual void unbind(EventHandler* channel) throw(ExecutionModelException&);
    virtual void open(CPUReservation* reserve = 0,CPUQoS* qos = 0) throw(ExecutionModelException&);
   

    virtual void shutdown() throw(ExecutionModelException&);
    void shutdown_i(bool reactorEvent) throw(ExecutionModelException&);

    bool isShutdown();
    
    virtual ExecutionModelResources& resources();

    virtual ExecutionModelResources* getResources() throw (ExecutionModelException&);
   

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;    
    ThreadPerConnectionEnginePtr m_tpc_i;
    bool m_debugThreadPerConnection;    
    //TPCTask* m_tpc;
    bool m_close;

   
};


#endif	/* _THREADPERCONNECTION_H */

