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
 * File:   TPRTask.cpp
 * Author: rmartins
 * 
 * Created on December 9, 2009, 12:23 PM
 */

#include "TPRTask.h"

TPRTask::TPRTask(ThreadPerRequest& tpr) :
m_tpr(tpr)/*,m_executing(false)*/, m_closed(false), m_request(0), m_condition(m_lock),
m_executor(0) {
    //m_lock = new ACE_SYNCH_MUTEX();
    //m_condition = new ACE_SYNCH_CONDITION(*m_lock);
}

TPRTask::~TPRTask() {
}

void TPRTask::schedule(Executor* executor, Request* request) {
    ACE_GUARD(ACE_SYNCH_MUTEX, ace_mon, m_lock);
    if (m_request != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)request not null (%d)!\n"), isExecuting()));
        /// throw exception            
        return;
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Request=%@\n"), request));
    m_request = request;
    m_executor = executor;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)before awake\n")));

    //this->awake();
    m_condition.signal();
    //ace_mon.release();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)after awake\n")));
    ///make sure it's processing
    //this->waitForExecutionStart();
}

/*virtual void awake()
{
    ACE_GUARD(ACE_SYNCH_MUTEX, ace_mon, m_lock);
    m_condition.signal();        

}*/

bool TPRTask::isExecuting() {
    //return m_executing;
    return (m_request != 0);
}

int TPRTask::open(void *args) {
    return activate();
}

int TPRTask::svc(void) {
    while (!m_closed) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)task: wait for input\n")));
        ACE_GUARD_RETURN(ACE_SYNCH_MUTEX, ace_mon, m_lock, -1);
        waitForExecution();
        if (m_request == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)no args for task\n")));
            ace_mon.release();
            continue;
        }
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)after args\n")));
        //m_executing = true;
        Request* request = m_request;
        Executor* executor = m_executor;
        ace_mon.release();
        schedule_i(executor, request);
        //m_executing = false;
        clearExecution();
        onExecutionFinish(executor, request);
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)thread end\n")));
    return 0;
}

void TPRTask::clearExecution() {
    m_request = 0;
    m_executor = 0;
}

void TPRTask::waitForExecution() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)wait=%d\n"), m_condition.wait(0)));
}

void TPRTask::onExecutionFinish(Executor* executor, Request* request) {
    //inform TPR
    m_tpr.onExecutionFinish(this, executor, request);
}

//interface - must be override

void TPRTask::schedule_i(Executor* executor, Request* request) {
    executor->execute(request);
}

/// Synchronization part II, after setting the request and executor,
/// let's get sure that this task is executing
//virtual void waitForExecutionStart()
//{
//
//}

int TPRTask::activate(CPUReservation* reserve, CPUQoS* qos,
        long flags,
        int n_threads,
        int force_active,
        long priority,
        int grp_id,
        ACE_Task_Base *task,
        ACE_hthread_t thread_handles[],
        void *stack[],
        size_t stack_size[],
        ACE_thread_t thread_ids[],
        const char* thr_name[]) {
    return ACE_Task_Base::activate(flags, n_threads, force_active, priority,
            grp_id, task, thread_handles,
            stack, stack_size, thread_ids, thr_name);

}

