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
 * File:   DynamicThreadPoolTask.cpp
 * Author: rmartins
 * 
 * Created on March 9, 2010, 10:21 PM
 */

#include "DynamicThreadPoolTask.h"
#include <euryale/threading/tp/ThreadPool.h>

DynamicThreadPoolTask::DynamicThreadPoolTask(ThreadPool& tp,ACE_Thread_Manager* manager) :
StaticThreadPoolTask(tp,manager)
{
}

DynamicThreadPoolTask::~DynamicThreadPoolTask()
{
}

int DynamicThreadPoolTask::svc_i(void)
{
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Dyn.Thread entered(%d)\n",gettid());

    if(m_tp.increaseDynamicCount() == -1){
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Dyn.Thread exit(-1) (%d)\n",gettid());
        return -1;
    }
    
    while (!m_tp.isShutdown() && !isDynamicThreadFinish()) {
        m_tp.join();
    }
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Dyn.Thread exit(%d)\n"),gettid()));
    /*
     * Thread already has decrease the dyn. TP counter
     */
    return 0;
}

/**
 * This method is used to check if the current dynamic thread should exit the
 * execution cycle.
 *
 * @return ...
 */
bool DynamicThreadPoolTask::isDynamicThreadFinish()
{
    return !m_tp.checkValidityDynamicThread();
}
