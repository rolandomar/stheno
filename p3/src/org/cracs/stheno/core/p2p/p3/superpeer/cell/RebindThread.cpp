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
 * File:   RebindThread.cpp
 * Author: rmartins
 * 
 * Created on September 3, 2010, 11:19 AM
 */

#include "RebindThread.h"
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>

RebindThread::RebindThread(Cell* cell, ACE_Thread_Manager* manager) :
m_cell(cell), m_manager(manager), Task(manager) {
}

RebindThread::~RebindThread() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::~TPCTask()\n")));
}

int RebindThread::svc(void) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - entered(%d)\n"),gettid()));
    m_handle = ACE_OS::thr_self();
    while (!m_cell->isClosed()) {
        if (!m_cell->isRebinding()) {
            break;
        }

    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RebindThread::svc() -  EXIT(%d)\n"), gettid()));
    //delete this;
    return 0;
}

ACE_thread_t RebindThread::getHandle() {
    return m_handle;
}
