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
 * File:   ThreadPool.h
 * Author: rmartins
 *
 * Created on December 14, 2009, 6:24 PM
 */

#ifndef _THREADPOOL_H
#define	_THREADPOOL_H

#include <ace/Atomic_Op.h>

#include <euryale/Types.h>
#include <euryale/event/Event.h>
#include <euryale/threading/Task.h>



class TPTask;
class StaticThreadPoolTask;
class DynamicThreadPoolTask;
#include <euryale/threading/ExecutionModel.h>

class ThreadPool : public ExecutionModel {
public:
    friend class StaticThreadPoolTask;
    friend class DynamicThreadPoolTask;

    ThreadPool(ExecutionModelResources* resources) throw(ExecutionModelException&);
    ThreadPool() throw(ExecutionModelException&);
    virtual ~ThreadPool();
    
    virtual void open() throw(ExecutionModelException&);
    virtual void open(UInt staticTasksNo, UInt maxDynamicTasks) throw(ExecutionModelException&);
    virtual void join(Event* event = 0, ACE_Time_Value* timeout = 0) throw(ExecutionModelException&)= 0;

    //not defined here, because is a general class
    virtual void bind(EventHandler* channel) throw(ExecutionModelException&);
    virtual void unbind(EventHandler* channel) throw(ExecutionModelException&);


    void shutdown() throw(ExecutionModelException&);
    bool isShutdown();

    UInt getDynamicThreadCount();
    UInt getMaxDynamicThreads();
    int setMaxDynamicThreads(UInt count);

protected:

    virtual void run();

    int checkDynamicThreads();

    bool checkValidityDynamicThread();

    int decreaseDynamicCount();

    int increaseDynamicCount();

    //TPTask* m_task;
    ACE_Thread_Manager m_staticThrManager;
    ACE_Thread_Manager m_dynamicThrManager;
    ACE_SYNCH_RECURSIVE_MUTEX m_mutex;
    DynamicThreadPoolTask* m_dynamicTask;
    StaticThreadPoolTask* m_staticTask;
    UInt m_maxDynamicTasks;
    UInt m_currentDynamicTasks;
    UInt m_staticTasks;
    bool m_close;

};

#endif	/* _THREADPOOL_H */

