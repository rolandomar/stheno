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
 * File:   TPCTask.h
 * Author: rmartins
 *
 * Created on December 9, 2009, 11:05 PM
 */

#ifndef _TPCTASK_H
#define	_TPCTASK_H

//#include <ace/Task.h>
#include <ace/Reactor.h>



#include <euryale/Conf.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/threading/Task.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/threading/tpc/ThreadPerConnectionEngine.h>
//template <class SOCK>

class TPCTask: public Task{
public:
    TPCTask(ThreadPerConnectionEnginePtr& tpc,ACE_Thread_Manager* manager = 0);
    virtual ~TPCTask();

    virtual int svc_i(void*);

    //ACE_thread_t getHandle();               

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    bool m_closed;    
    ThreadPerConnectionEnginePtr m_tpc;
    ACE_thread_t m_handle;
    
    virtual int close (u_long flags = 0);

};

#endif	/* _TPCTASK_H */

