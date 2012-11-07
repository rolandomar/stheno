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
 * File:   TimerManager.h
 * Author: rmartins
 *
 * Created on October 17, 2010, 12:55 AM
 */

#ifndef TIMERMANAGER_H
#define	TIMERMANAGER_H
#include <euryale/threading/Task.h>
#include <ace/Timer_Queue.h>

class TimerManager;
class TimerTask: public Task{
public:
    TimerTask(TimerManager* tm);
    virtual int svc_i(void*);
    
    virtual int close (u_long flags = 0){return 0;}
protected:
    TimerManager* m_tm;
};



class TimerManager {
public:
    TimerManager(ACE_Timer_Queue* tq);    
    virtual ~TimerManager();
    
    int open(){
        if(m_task != 0){
            return -1;
        }
        m_task = new TimerTask(this);
        return m_task->activate();
    }
    
    bool isClosed(){
        return m_closed;
    }
    void close(){
        m_closed = true;   
        m_threadManager.wait();
        delete m_task;
        m_task = 0;
    }
    
    ACE_Timer_Queue* getTimerQueue(){
        return m_tq;
    }
    
    ACE_Thread_Manager* getThreadManager(){
        return &m_threadManager;
    }
    
private:
    ACE_Thread_Manager m_threadManager;
    TimerTask* m_task;
    bool m_closed;
    ACE_Timer_Queue* m_tq;
};

#endif	/* TIMERMANAGER_H */

