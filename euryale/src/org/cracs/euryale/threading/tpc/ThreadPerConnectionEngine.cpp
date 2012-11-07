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
 * File:   TPCPerConnectionEngine.cpp
 * Author: rmartins
 * 
 * Created on February 12, 2011, 4:44 PM
 */

#include "ThreadPerConnectionEngine.h"

#include <euryale/runtime/RuntimeConfiguration.h>
#include <euryale/threading/tpc/TPCTask.h>
#include <euryale/common/Backtrace.h>
#include <ace/Dev_Poll_Reactor.h>

//template <class SOCK>

ThreadPerConnectionEngine::ThreadPerConnectionEngine(ExecutionModelResources* resources) throw (ExecutionModelException&) :
ExecutionModel(resources), m_owner(0), /*m_tpc(0),*/ m_close(false),
m_handler(0), m_hasThread(false) {
    m_debugThreadPerConnection = RuntimeConfiguration::isClassDebugged("ThreadPerConnectionEngine");
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::ThreadPerConnectionEngine() this=%@\n"), this));
    }
}

ThreadPerConnectionEngine::ThreadPerConnectionEngine() throw (ExecutionModelException&) :
/*m_tpc(0)*/m_owner(0), m_close(false),
m_handler(0), m_hasThread(false) {
    m_debugThreadPerConnection = RuntimeConfiguration::isClassDebugged("ThreadPerConnectionEngine");
    m_resources->setReactor(allocateReactorImpl(), true);
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::ThreadPerConnectionEngine() this=%@\n"), this));
    }
}

ThreadPerConnectionEngine::~ThreadPerConnectionEngine() {
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::~ThreadPerConnectionEngine() this=%@\n"), this));
    }

    //m_tpcThrManager.wait_on_exit(0);
    shutdown();
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::~ThreadPerConnectionEngine() - this=%@ end\n"), this));
    }
}

int ThreadPerConnectionEngine::handleCloseImpl(EventHandler* handler, ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    /*if (m_tpc != 0) {
        m_tpc->close();
        m_tpc = 0;
    }*/
    this->shutdown();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::::onEventHandlerClose\n");
    //m_handler = 0;
    //shutdown_i(false);
    //doesnt matter
    return 0;
}

void ThreadPerConnectionEngine::open(CPUReservation* reserve, CPUQoS* qos, TPCTask* task) throw (ExecutionModelException&) {
    /*m_tpc = task;
    if (m_tpc != 0) {
        //int ret = m_tpc->activate(0, 0, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1);
        int ret = m_tpc->activate(reserve, qos, THR_NEW_LWP | THR_DETACHED, 1);
        if (ret != 0) {
            printf("=====>ERROR: ThreadPerConnectionEngine::open\n");
            throw (ExecutionModelException(ExecutionModelException::INITIALIZATION_ERROR));
        }
    }*/
    if (m_owner == 0) {
        ACE_hthread_t thread_handles[1];
        int ret = task->activate(reserve, qos, THR_NEW_LWP | THR_DETACHED, 1, thread_handles);
        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::open this=%@\n"), this));
            throw (ExecutionModelException(ExecutionModelException::INITIALIZATION_ERROR));
        } else {
            m_owner = thread_handles[0];
            m_hasThread = true;
            if (m_debugThreadPerConnection) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::open ok this=%@ owner(%Q)\n"), this, m_owner));
            }
        }
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::open NOK this=%@ owner(%Q)\n"), this, m_owner));
        throw (ExecutionModelException(ExecutionModelException::INITIALIZATION_ERROR));
    }
    return;
}

void ThreadPerConnectionEngine::shutdown() throw (ExecutionModelException&) {
    //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    shutdown_i(true);
}

void ThreadPerConnectionEngine::shutdown_i(bool reactorEvent) throw (ExecutionModelException&) {
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::shutdown_i()\n")));
    }
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_close) {
        if (m_debugThreadPerConnection) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::shutdown_i() - already closed\n")));
        }
        return;
    }
    m_close = true;
    if (m_resources->getReactor() != 0) {
        m_resources->getReactor()->close();
    }
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::shutdown_i() end\n")));
    }
    return;
}

void ThreadPerConnectionEngine::join(Event* event, ACE_Time_Value* timeout) throw (ExecutionModelException&) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() - before lock!\n")));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() - fater lock!\n")));
    if (m_close) {
        if (m_debugThreadPerConnection) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() - closed!\n")));
        }
        throw (ExecutionModelException(ExecutionModelException::CLOSED_ERROR));
    }
    if (m_handler == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() - no handler!\n")));
        throw (ExecutionModelException(ExecutionModelException::NO_HANDLER));
    }

    if (m_owner == 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() OWER i szERO\n")));
        //we dont a pre-spawned thread to handle this ec
        //Outside task will be handling this
        m_owner = ACE_OS::thr_self();
        m_hasThread = true;
        ace_mon.release();
        join_i(event, timeout);
        {
            ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
            m_hasThread = false;
            m_owner = 0;
        }
    } else {
        //It's the tpctask ownes the ec, so it can enter
        if (m_owner != 0 && ACE_OS::thr_self() == m_owner) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() OWER %Q\n"),m_owner));
            ace_mon.release();
            join_i(event, timeout);
            return;
        } else {
            //EC task handles
            if (m_hasThread) {
                //There is already a task processing this ec
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join() - already has thread!\n")));
                throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
            }
        }
        throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
    }
}

void ThreadPerConnectionEngine::bind(EventHandler* channel) throw (ExecutionModelException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_handler != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::open:() bad binding\n")));
        throw (ExecutionModelException(ExecutionModelException::BAD_BINDING));
    }
    m_handler = channel;
    m_handler->attachSpecilization(this);
    ACE_Reactor* reactor = m_resources->getReactor();
    if (reactor == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::open:reactor failed\n")));
        throw (ExecutionModelException(ExecutionModelException::BAD_BINDING));
    }
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::open:() setting reactor=%@ channel=%@\n"),reactor,channel));
    channel->reactor(reactor);
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnectionEngine: bind() OK!\n")));
    }
}

void ThreadPerConnectionEngine::unbind(EventHandler* channel) throw (ExecutionModelException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_handler = 0;
}

ACE_Reactor* ThreadPerConnectionEngine::allocateReactorImpl(void) {
    /*ACE_Dev_Poll_Reactor* impl = new ACE_Dev_Poll_Reactor(
            ACE::max_handles(),
            0,
            (ACE_Sig_Handler*) 0,
            (ACE_Timer_Queue*) 0,
            0,
            0,
            0,
            ACE_Select_Reactor_Token::FIFO);    
    return new ACE_Reactor(impl);*/
    ACE_Reactor_Impl *impl = 0;
    ACE_NEW_RETURN(impl,
            ACE_TP_Reactor(ACE::max_handles(),
            1,
            (ACE_Sig_Handler*) 0,
            (ACE_Timer_Queue*) 0,
            0,
            ACE_Select_Reactor_Token::FIFO),
            0);
    return new ACE_Reactor(impl);
    /*ACE_Reactor_Impl *impl = 0;
    ACE_NEW_RETURN(impl,
            ACE_Select_Reactor(ACE::max_handles(),       
            false,
            (ACE_Sig_Handler*) 0,
            (ACE_Timer_Queue*) 0,
            0,
            0,
            ACE_Select_Reactor_Token::FIFO),
            0);
    return new ACE_Reactor(impl);
    //return (ACE_Reactor*) impl;*/
}

bool ThreadPerConnectionEngine::isShutdown() {
    return m_close;
}

ACE_Event_Handler* ThreadPerConnectionEngine::getHandler() {
    return m_handler;
}

void ThreadPerConnectionEngine::join_i(Event* event, ACE_Time_Value* timeout) throw (ExecutionModelException&) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i\n")));
    if (m_handler == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join_i - ex:handler == 0\n")));
        throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
        //return -1;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i - 1\n");
    ACE_Event_Handler* handler = m_handler;
    if (handler == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join_i - ex2:handler == 0\n")));
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::svc:handler = 0\n");
        throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
        //return -1;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i - 2\n");
    ACE_Reactor* reactor = m_resources->getReactor();
    if (reactor == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join_i - ex: reactor == 0\n")));
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::svc: reactor = 0\n");
        throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
        //return -1;
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i - 3\n");
    reactor->owner(ACE_Thread::self());
    //ACE_Time_Value nowait(ACE_OS::gettimeofday());
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::handle_events\n");
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::join_i() - BEFORE HANDLE EVENTS this=%@\n"), this));
    int result = reactor->handle_events(timeout);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnectionEngine::join_i() - AFTER HANDLE EVENTS this=%@\n"), this));
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i - 5\n");
    if (result == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnectionEngine::join_i - ex: result == -1\n")));
        throw (ExecutionModelException(ExecutionModelException::JOIN_FAILED));
        //return -1;
    }
    //return 0;
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnectionEngine::join_i ok\n")));
    return;
}

