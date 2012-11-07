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
 * File:   NotifyThread.h
 * Author: rmartins
 *
 * Created on September 8, 2010, 4:22 PM
 */

#ifndef NOTIFYTHREAD_H
#define	NOTIFYTHREAD_H

#include <euryale/Conf.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/threading/Task.h>

class Cell;
class NotifyThread: public Task{
public:
    static const int PERIOD = 1000;

    NotifyThread(Cell* cell,ACE_Thread_Manager* manager = 0);
    virtual ~NotifyThread();

    virtual int svc_i(void*);

    void shutdown();

protected:    
    bool m_stopped;
    Cell* m_cell;
    
    virtual int close (u_long flags = 0){return 0;}

};

#endif	/* NOTIFYTHREAD_H */

