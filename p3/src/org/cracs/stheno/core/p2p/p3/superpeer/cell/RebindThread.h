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
 * File:   RebindThread.h
 * Author: rmartins
 *
 * Created on September 3, 2010, 11:19 AM
 */

#ifndef REBINDTHREAD_H
#define	REBINDTHREAD_H

#include <euryale/Conf.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/threading/Task.h>

class Cell;
class RebindThread: public Task{
public:
    RebindThread(Cell* cell,ACE_Thread_Manager* manager = 0);
    virtual ~RebindThread();

    virtual int svc(void);

    ACE_thread_t getHandle();
    
protected:
    ACE_Thread_Manager* m_manager;
    Cell* m_cell;
    ACE_thread_t m_handle;

};


#endif	/* REBINDTHREAD_H */

