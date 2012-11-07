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
 * File:   ThreadPerRequest.cpp
 * Author: rmartins
 * 
 * Created on December 9, 2009, 12:26 PM
 */

#include "ThreadPerRequest.h"
#include <euryale/threading/tpr/TPRTask.h>
#include <euryale/threading/Executor.h>

ThreadPerRequest::ThreadPerRequest() :
m_task(0)
{
}

ThreadPerRequest::~ThreadPerRequest()
{
}

void ThreadPerRequest::execute(Executor* executor, Request* request, ACE_Time_Value* timeout)
{
    /// get task
    TPRTask* task = getTask();
    if (task == 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Error no task\n")));
        /// throw
        return;
    }
    task->schedule(executor, request);
}

int ThreadPerRequest::open(void *args)
{
    if (m_task == 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)new task\n")));
        m_task = new TPRTask(*this);
        return m_task->open();
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)error: already have task\n")));
    return -1;

}

