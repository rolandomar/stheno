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
 * File:   NotifyThread.cpp
 * Author: rmartins
 * 
 * Created on September 8, 2010, 4:22 PM
 */

#include "NotifyThread.h"
#include "Cell.h"

NotifyThread::NotifyThread(Cell* cell, ACE_Thread_Manager* manager) :
m_cell(cell), Task(manager), m_stopped(false) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: NotifyThread::NotifyThread()\n")));
}

NotifyThread::~NotifyThread() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: NotifyThread::~NotifyThread()\n")));
}

int NotifyThread::svc_i(void*) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: NotifyThread::svc() - entered(%d)\n"), gettid()));
    while (!m_cell->isClosed() && !m_stopped) {
        if (!m_cell->isRebinding()) {
            if (m_cell->isStarted()) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: NotifyThread::svc() -  notify\n")));
                m_cell->sendNotify();
            }
        }
        SleepHelper::sleep(PERIOD);

    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: NotifyThread::svc() -  EXIT(%d)\n"), gettid()));
    //delete this;
    return 0;
}

void NotifyThread::shutdown() {
    m_stopped = true;
}