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
 * File:   TPRTask.h
 * Author: rmartins
 *
 * Created on December 9, 2009, 12:23 PM
 */

#ifndef _TPRTASK_H
#define	_TPRTASK_H

#include <ace/Task.h>

#include <euryale/threading/tpr/ThreadPerRequest.h>
#include <euryale/threading/Executor.h>
#include <euryale/Conf.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>

class Request;

class TPRTask : public ACE_Task_Base {
public:
    TPRTask(ThreadPerRequest& tpr);
    virtual ~TPRTask();

    void schedule(Executor* executor, Request* request);
    /*virtual void awake()
    {
        ACE_GUARD(ACE_SYNCH_MUTEX, ace_mon, m_lock);
        m_condition.signal();        

    }*/

    virtual bool isExecuting();
    virtual int open(void *args = 0);
    virtual int svc(void);
    
protected:
    ThreadPerRequest& m_tpr;
    //bool m_executing;
    bool m_closed;
    Request* m_request;
    Executor* m_executor;
    ACE_SYNCH_MUTEX m_lock;
    /// Condition variable used to
    ACE_SYNCH_CONDITION m_condition;

    virtual void clearExecution();

    virtual void waitForExecution();
    virtual void onExecutionFinish(Executor* executor, Request* request);
    
    virtual void schedule_i(Executor* executor, Request* request);

    virtual int activate(CPUReservation* reserve = 0, CPUQoS* qos = 0,
            long flags = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
            int n_threads = 1,
            int force_active = 0,
            long priority = ACE_DEFAULT_THREAD_PRIORITY,
            int grp_id = -1,
            ACE_Task_Base *task = 0,
            ACE_hthread_t thread_handles[] = 0,
            void *stack[] = 0,
            size_t stack_size[] = 0,
            ACE_thread_t thread_ids[] = 0,
            const char* thr_name[] = 0);
};

#endif	/* _TPRTASK_H */

