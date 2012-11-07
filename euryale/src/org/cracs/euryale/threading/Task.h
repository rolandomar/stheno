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
 * File:   Task.h
 * Author: rmartins
 *
 * Created on December 9, 2009, 11:24 AM
 */

#ifndef _TASK_H
#define	_TASK_H

#include <ace/Task.h>
#include <ace/OS_NS_Thread.h>
#include <ace/Sched_Params.h>
#include <euryale/Conf.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <euryale/event/Event.h>
#include <euryale/common/exception/Exception.h>

#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/threading/exception/TaskException.h>
//#include <ace/Barrier.h>


class TaskParams;
class Task : public ACE_Task_Base {
public:
    Task();
    Task(ACE_Thread_Manager* manager);    
    virtual ~Task();
    
    virtual int close (u_long flags = 0) = 0;
    
    virtual int svc_i(void* arg) = 0;

    static void setThreadScheduling(ACE_hthread_t handle, CPUQoS* qos) throw (TaskException&);
    void getThreadScheduling(ACE_hthread_t handle, CPUQoS*& qos) throw (TaskException&);
    static void setQoS(CPUReservation* reserve,CPUQoS* qos) throw (TaskException&);

    TaskParams* getTaskParams();

    virtual int activate(
            CPUReservation* reserve = 0,
            CPUQoS* qos = 0,
            long flags = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
            int n_threads = 1,
            //int force_active = 0,            
            //long priority = ACE_DEFAULT_THREAD_PRIORITY,
            //int grp_id = -1,
            //ACE_Task_Base *task = 0,
            ACE_hthread_t thread_handles[] = 0,
            void *stack[] = 0,
            size_t stack_size[] = 0);
            //ACE_thread_t thread_ids[] = 0,
            //const char* thr_name[] = 0);
    
    virtual int reactivate(
            long flags = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
            int n_threads = 1,            
            ACE_hthread_t thread_handles[] = 0,
            void *stack[] = 0,
            size_t stack_size[] = 0);
    
    

protected:    
    ACE_hthread_t* m_threadHandlers;
    TaskParams* m_taskParams;          
    bool m_debugTask;
    
    virtual int svc(void);
};

#endif	/* _TASK_H */

