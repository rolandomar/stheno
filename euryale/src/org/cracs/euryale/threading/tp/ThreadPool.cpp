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
 * File:   ThreadPool.cpp
 * Author: rmartins
 * 
 * Created on December 14, 2009, 6:24 PM
 */

#include "ThreadPool.h"
#include <euryale/threading/tp/DynamicThreadPoolTask.h>
#include <euryale/threading/tp/StaticThreadPoolTask.h>

ThreadPool::ThreadPool(ExecutionModelResources* resources) throw (ExecutionModelException&) :
ExecutionModel(resources) {
    m_close = false;
    m_dynamicTask = new DynamicThreadPoolTask(*this, &m_dynamicThrManager);
    m_staticTask = new StaticThreadPoolTask(*this, &m_staticThrManager);
    //m_task = new TPTask(*this);
}

ThreadPool::ThreadPool() throw (ExecutionModelException&) {
    m_close = false;
    m_dynamicTask = new DynamicThreadPoolTask(*this, &m_dynamicThrManager);
    m_staticTask = new StaticThreadPoolTask(*this, &m_staticThrManager);
    //m_task = new TPTask(*this);
}

ThreadPool::~ThreadPool() {
    delete m_staticTask;
    delete m_dynamicTask;
    //delete m_task;
}

void ThreadPool::open() throw (ExecutionModelException&) {
    open(2, 0);
}
//int ThreadPool::open(int size,Runnable* run) {

void ThreadPool::open(UInt staticTasksNo, UInt maxDynamicTasks) throw (ExecutionModelException&) {
    m_maxDynamicTasks = maxDynamicTasks;
    m_currentDynamicTasks = 0;
    m_staticTasks = staticTasksNo;
    //return m_staticTask->activate(0, 0, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_staticTasks);
    if (m_staticTask->activate(0, 0, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, m_staticTasks) != 0) {
        throw (ExecutionModelException(ExecutionModelException::INITIALIZATION_ERROR));
    }
    //return m_task->open(size,run);
    //return  m_staticTask->open();
}

void ThreadPool::shutdown() throw (ExecutionModelException&) {
    //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, -1);
    ACE_GUARD_REACTION(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex,
            throw (ExecutionModelException(ExecutionModelException::LOCK_ERROR)));
    m_close = true;
    m_staticTask->wait();
    m_dynamicTask->wait();
    return;
    //return 0;
    //return m_task->shutdown();
}

int ThreadPool::checkDynamicThreads() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, -1);
    if (m_currentDynamicTasks < m_maxDynamicTasks) {
        int threadCount = m_maxDynamicTasks - m_currentDynamicTasks;
        //return m_dynamicTask->activate(0, 0, THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, threadCount,1);
        return m_dynamicTask->reactivate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, threadCount);
    }
    return 0;
}

bool ThreadPool::checkValidityDynamicThread() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, false);
    if (m_currentDynamicTasks > m_maxDynamicTasks) {
        decreaseDynamicCount();
        return false;
    }
    return true;
}

int ThreadPool::decreaseDynamicCount() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, -1);
    if (m_currentDynamicTasks == 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)decreaseDynamicCount -1\n")));
        return -1;
    }
    return --m_currentDynamicTasks;
}

int ThreadPool::increaseDynamicCount() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, -1);
    if (m_currentDynamicTasks >= m_maxDynamicTasks) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)increaseDynamicCount -1\n")));
        return -1;
    }
    return ++m_currentDynamicTasks;
}

bool ThreadPool::isShutdown() {
    return m_close;
}

UInt ThreadPool::getDynamicThreadCount() {
    return m_currentDynamicTasks;
}

UInt ThreadPool::getMaxDynamicThreads() {
    return m_maxDynamicTasks;
}

int ThreadPool::setMaxDynamicThreads(UInt count) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_mutex, -1);
    m_maxDynamicTasks = count;
    return checkDynamicThreads();
}

void ThreadPool::run() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ThreadPool:run (%d)\n"), gettid()));
    sleep(1);
}

//not defined here, because is a general class

void ThreadPool::bind(EventHandler* channel) throw (ExecutionModelException&) {
}

void ThreadPool::unbind(EventHandler* channel) throw (ExecutionModelException&) {
}