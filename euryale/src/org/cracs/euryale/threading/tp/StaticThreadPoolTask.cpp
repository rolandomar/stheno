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
 * File:   StaticThreadPoolTask.cpp
 * Author: rmartins
 * 
 * Created on March 9, 2010, 10:20 PM
 */

#include "StaticThreadPoolTask.h"

StaticThreadPoolTask::StaticThreadPoolTask(ThreadPool& tp,ACE_Thread_Manager* manager) :
m_tp(tp),m_manager(manager),Task(manager)
{
}

StaticThreadPoolTask::~StaticThreadPoolTask()
{
}

int StaticThreadPoolTask::svc_i(void*)
{
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Static Thread entered(%d)\n"),gettid()));
    while (!m_tp.isShutdown()) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Static Thread cycle(%d)\n",gettid());
        m_tp.join();
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Static Thread end cycle(%d)\n",gettid());
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Static Thread exit(%d)\n"),gettid()));
    return 0;
}

