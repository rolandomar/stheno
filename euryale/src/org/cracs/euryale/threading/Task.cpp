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
 * File:   Task.cpp
 * Author: rmartins
 * 
 * Created on December 9, 2009, 11:24 AM
 */

#include "Task.h"
#include "TaskParams.h"
#include <euryale/qos/MedusaClient.h>

Task::Task() :
m_threadHandlers(0), m_taskParams(0), m_debugTask(false) {
    m_debugTask = RuntimeConfiguration::isClassDebugged("Task");
}

Task::Task(ACE_Thread_Manager* manager) : ACE_Task_Base(manager),
m_threadHandlers(0), m_taskParams(0), m_debugTask(false) {
    m_debugTask = RuntimeConfiguration::isClassDebugged("Task");
}

Task::~Task() {
    if (m_taskParams != 0) {
        delete m_taskParams;
    }
}

void Task::setThreadScheduling(ACE_hthread_t handle, CPUQoS* qos) throw (TaskException&) {
    switch (qos->getPolicy()) {
        case CPUQoS::SCHEDULE_FIFO:
        case CPUQoS::SCHEDULE_RR:
        case CPUQoS::SCHEDULE_OTHER:
        {
            CPUPriorityQoS* qosPriority = static_cast<CPUPriorityQoS*> (qos);
            int policy = qosPriority->getPolicy();
            int priority = qosPriority->getPriority();
            //ACE_hthread_t handle;
            //ACE_OS::thr_self(handle);
            if (ACE_OS::thr_setprio(ACE_OS::thr_self(), priority, policy) != 0) {

                printf("Failed to set priority to task %lu %d %d!", handle, priority, policy);
                perror("task failure:");
                throw TaskException("ACE_OS::thr_setprio failed!");
            }else{
                //if(m_debugTask){
                    printf("Task QoS(%d,%d)\n",policy,priority);
                //}
            }

            /*ACE_Sched_Params s(CPUQoS::SCHEDULE_FIFO,50);                
            if(ACE_OS::sched_params(s,gettid()!=0)){
                throw new TaskException("ACE_OS::thr_setprio failed!");
            }*/
            return;
        }
        case CPUQoS::SCHEDULE_DEADLINE:
        default:
        {
            throw TaskException("ACE_OS::thr_setprio failed! Not supported");
        }

    }
}

void Task::getThreadScheduling(ACE_hthread_t handle, CPUQoS*& qos) throw (TaskException&) {
    int priority = -1;
    int policy = -1;
    if (ACE_OS::thr_getprio(handle, priority, policy) != 0) {
        perror("=");
        throw TaskException("ACE_OS::thr_getprio failed!");
    }
    qos = new CPUPriorityQoS(policy, priority);
}

TaskParams* Task::getTaskParams() {
    return m_taskParams;
}

int Task::svc(void) {
    if (m_taskParams != 0) {
        CPUQoS* qos = m_taskParams->getCPUQoS();
        CPUReservation* reserve = m_taskParams->getCPUReservation();
        try {
            setQoS(reserve, qos);
        } catch (TaskException& ex) {
            ACE_DEBUG((LM_CRITICAL, ACE_TEXT("(%t|%T)ERROR: Task::svc() - failed to setQoS, exiting thread|")));
            return -1;
        }
    }
    return svc_i(0);
}

void Task::setQoS(CPUReservation* reserve, CPUQoS* qos) throw (TaskException&) {

    if (qos != 0) {
        //if (m_debugTask) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Task::setQoS() - TID(%d) QoS(%@)\n"), gettid(), qos));
        //}
        
        //new!!!
        //setThreadScheduling(ACE_OS::thr_self(), qos);
    }
    if (reserve != 0) {
        //if (m_debugTask) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Task::setQoS() - set reserve, TID(%d) QoS(%@)\n"), gettid(), qos));
        //}
        try {
            GlobalMedusaClient::instance()->setTaskInCPUPartition(gettid(),
                    reserve->getCpusetDomain(), reserve->getScheduleDomain(), qos->duplicate());
        } catch (CgroupException& ex) {
            //if (m_debugTask) {
                ACE_DEBUG((LM_CRITICAL, ACE_TEXT("(%t|%T)ERROR: Task::setQoS() - reservation failed, TID(%d) QoS(%@)\n"), gettid(), qos));
            //}
            throw TaskException(TaskException::CGROUP_FAILURE);
        }
    }
}

int Task::activate(
        CPUReservation* reserve,
        CPUQoS* qos,
        long flags,
        int n_threads,
        //int force_active,
        //long priority,
        //int grp_id,
        //ACE_Task_Base *task,
        ACE_hthread_t thread_handles[],
        void *stack[],
        size_t stack_size[]) {
    //ACE_thread_t thread_ids[],
    //const char* thr_name[]) {
    //bool threadHandlesLocal = false;
    //if (/*reserve != 0 && */qos != 0/*&& thread_handles == 0*/) {
    //  threadHandlesLocal = true;
    //thread_handles = new ACE_hthread_t[n_threads];
    //}

    /*if (m_threadHandlers != 0) {
        m_threadHandlers = new ACE_hthread_t[n_threads];
    } else {
        
    }*/

    if (m_taskParams != 0) {
        return -1;
    }
    m_taskParams = new TaskParams(qos, reserve);

    /*TaskParams* tp = new TaskParams(qos,reserve);
    stack = new void*[n_threads];
    for(int i=0; i < n_threads; i++){
        stack[i]=tp->duplicate();
    }
    delete tp;*/


    //m_barrier = new ACE_Thread_Barrier(n_threads);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%@\n",m_barrier);
    
    //new!!!!!!!!!!!11
    long prio = ACE_DEFAULT_THREAD_PRIORITY;
    if(qos != NULL){
    switch (qos->getPolicy()) {
        case CPUQoS::SCHEDULE_FIFO:
        {
            CPUPriorityQoS* qosPriority = static_cast<CPUPriorityQoS*> (qos);
            prio = qosPriority->getPriority();
            ACE_SET_BITS (flags, THR_EXPLICIT_SCHED);        
            ACE_SET_BITS (flags, THR_SCHED_FIFO);        
            break;
        }
        case CPUQoS::SCHEDULE_RR:
        {            
            CPUPriorityQoS* qosPriority = static_cast<CPUPriorityQoS*> (qos);
            prio = qosPriority->getPriority();
            ACE_SET_BITS (flags, THR_EXPLICIT_SCHED);        
            ACE_SET_BITS (flags, THR_SCHED_RR);        
            break;
            
        }
        case CPUQoS::SCHEDULE_OTHER:
        {
            CPUPriorityQoS* qosPriority = static_cast<CPUPriorityQoS*> (qos);
            //int policy = qosPriority->getPolicy();
            prio = 0;//ACE_DEFAULT_THREAD_PRIORITY;//qosPriority->getPriority();
            ACE_SET_BITS (flags, THR_EXPLICIT_SCHED);        
            ACE_CLR_BITS (flags,THR_SCHED_RR);
            ACE_CLR_BITS (flags,THR_SCHED_FIFO);
            ACE_SET_BITS (flags, THR_SCHED_DEFAULT);                        
            break;                 
        }
        case CPUQoS::SCHEDULE_DEADLINE:
        default:
        {
            throw TaskException("ACE_OS::thr_setprio failed! Not supported");
        }

    }
        
    
    }
    
    int ret = ACE_Task_Base::activate(
            flags,
            n_threads,
            0,
            prio,//ACE_DEFAULT_THREAD_PRIORITY,
            -1,
            0,
            thread_handles,
            stack,
            stack_size,
            0,
            0);

    /*if (qos != 0) {
        for (int i = 0; i < n_threads; i++) {
            setThreadScheduling(m_threadHandlers[i], qos);
        }
    }*/
    /*if (reserve != 0 && qos != 0) {
        for (int i = 0; i < n_threads; i++) {
            ret = activateQoS(thread_handles[i], reserve, qos);
        }
    }*/

    /*if (threadHandlesLocal) {
        delete [] thread_handles;
    }*/
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%@.wait\n",m_barrier);


    return ret;

}

int Task::reactivate(
        long flags,
        int n_threads,
        ACE_hthread_t thread_handles[],
        void *stack[],
        size_t stack_size[]) {

    return ACE_Task_Base::activate(
            flags,
            n_threads,
            1,
            ACE_DEFAULT_THREAD_PRIORITY,
            -1,
            0,
            thread_handles,
            stack,
            stack_size,
            0,
            0);
}