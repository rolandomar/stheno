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
 * File:   StaticThreadPoolTask.h
 * Author: rmartins
 *
 * Created on March 9, 2010, 10:20 PM
 */

#ifndef _STATICTHREADPOOLTASK_H
#define	_STATICTHREADPOOLTASK_H

#include <euryale/threading/Task.h>
#include <euryale/threading/tp/ThreadPool.h>

class StaticThreadPoolTask: public Task{
public:
    StaticThreadPoolTask(ThreadPool& tp,ACE_Thread_Manager* manager);
    virtual ~StaticThreadPoolTask();

    virtual int svc_i(void*);

    virtual int close (u_long flags = 0){return 0;}
protected:
    ACE_Thread_Manager* m_manager;
    ThreadPool& m_tp;

};

#endif	/* _STATICTHREADPOOLTASK_H */

