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
 * File:   ThreadPerConnection.cpp
 * Author: rmartins
 * 
 * Created on December 9, 2009, 11:04 PM
 */

//#ifndef _THREADPERCONNECTION_CPP
//#define _THREADPERCONNECTION_CPP
#include "ThreadPerConnection.h"
#include <euryale/runtime/RuntimeConfiguration.h>
#include <euryale/threading/tpc/TPCTask.h>
#include <euryale/common/Backtrace.h>

//template <class SOCK>

ThreadPerConnection::ThreadPerConnection(ExecutionModelResources* resources) throw (ExecutionModelException&) :
m_close(false),
m_tpc_i(new ThreadPerConnectionEngine(resources)) {
    m_debugThreadPerConnection = RuntimeConfiguration::isClassDebugged("ThreadPerConnection");
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnection::ThreadPerConnection() this=%@\n"), this));
    }
}

ThreadPerConnection::ThreadPerConnection() throw (ExecutionModelException&) :
m_close(false),
m_tpc_i(new ThreadPerConnectionEngine()) {
    m_debugThreadPerConnection = RuntimeConfiguration::isClassDebugged("ThreadPerConnection");
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnection::ThreadPerConnection() this=%@\n"), this));
    }
}

ThreadPerConnection::~ThreadPerConnection() {
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPerConnection::~ThreadPerConnection() this=%@\n"), this));
    }
    shutdown();
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::~ThreadPerConnection() - this=%@ end\n"), this));
    }
}

void ThreadPerConnection::open(CPUReservation* reserve, CPUQoS* qos) throw (ExecutionModelException&) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::open() this=%@\n"),this));    
    TPCTask* task = new TPCTask(m_tpc_i, 0);
    if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::open() this=%@ task=%@\n"), this, task));
    }
    /*if (m_tpc != 0) {
        //int ret = m_tpc->activate(0, 0, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, 1);
        int ret = m_tpc->activate(reserve, qos, THR_NEW_LWP | THR_DETACHED, 1);
        if (ret != 0) {
            throw (ExecutionModelException(ExecutionModelException::INITIALIZATION_ERROR));
        }
    }*/
    try {
        m_tpc_i->open(reserve, qos, task);
    } catch (ExecutionModelException& ex) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnection::open()  fail to open this=%@ task=%@\n"), this, task));
        delete task;
        throw ex;
    }
    return;
}

void ThreadPerConnection::shutdown() throw (ExecutionModelException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    shutdown_i(true);
}

void ThreadPerConnection::shutdown_i(bool reactorEvent) throw (ExecutionModelException&) {
    if (m_debugThreadPerConnection) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::shutdown_i() this=%@\n"), this));
    }
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_close) {
        if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::shutdown_i() - already closed this=%@\n"), this));
        }
        //return 0;
        return;
    }
    m_close = true;
    m_tpc_i->shutdown();
    return;
}

void ThreadPerConnection::join(Event* event, ACE_Time_Value* timeout) throw (ExecutionModelException&) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::join() - before lock!\n")));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ThreadPerConnection::join() - fater lock!\n")));
    if (m_close) {
        if (m_debugThreadPerConnection) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: ThreadPerConnection::join() - closed!\n")));
        }
        throw (ExecutionModelException(ExecutionModelException::CLOSED_ERROR));
    }
    m_tpc_i->join(event, timeout);
}

void ThreadPerConnection::bind(EventHandler* channel) throw (ExecutionModelException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_tpc_i->bind(channel);
}

void ThreadPerConnection::unbind(EventHandler* channel) throw (ExecutionModelException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_tpc_i->unbind(channel);
}

bool ThreadPerConnection::isShutdown() {
    return m_close;
}

ACE_Event_Handler* ThreadPerConnection::getHandler() {
    return m_tpc_i->getHandler();
}

ExecutionModelResources& ThreadPerConnection::resources() {
    return m_tpc_i->resources();
}

ExecutionModelResources* ThreadPerConnection::getResources() throw (ExecutionModelException&) {
    return m_tpc_i->getResources();
}