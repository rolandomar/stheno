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
 * File:   TimerManager.cpp
 * Author: rmartins
 * 
 * Created on October 17, 2010, 12:55 AM
 */

#include "TimerManager.h"


TimerTask::TimerTask(TimerManager* tm):Task(tm->getThreadManager()),m_tm(tm){
    
}

int TimerTask::svc_i(void*){
    while(!m_tm->isClosed()){
        m_tm->getTimerQueue()->expire();
        ACE_Time_Value* t = 0;
        m_tm->getTimerQueue()->calculate_timeout(t);        
        if(t == 0){
            ACE_OS::sleep (ACE_Time_Value (1, 0));
        }else{
            ACE_OS::sleep(*t);
        }
    }
    return 0;
}


TimerManager::TimerManager(ACE_Timer_Queue* tq):m_tq(tq),m_closed(false),m_task(0) {
}

TimerManager::~TimerManager() {
    close();
    delete m_tq;
}

