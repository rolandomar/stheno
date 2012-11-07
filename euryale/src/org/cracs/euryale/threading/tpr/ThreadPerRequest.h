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
 * File:   ThreadPerRequest.h
 * Author: rmartins
 *
 * Created on December 9, 2009, 12:26 PM
 */

#ifndef _THREADPERREQUEST_H
#define	_THREADPERREQUEST_H


#include <ace/Time_Value.h>
#include <ace/Log_Msg.h>
//#include "TPRTask.h"



class Request;
class TPRTask;
class Executor;
class ThreadPerRequest {
public:
    ThreadPerRequest();
    
    virtual ~ThreadPerRequest();


    virtual int open(void *args = 0);

    /// this can create a new thread; take an existing
    /// thread from a pool; etc
    virtual void execute(Executor* executor, Request* request,ACE_Time_Value* timeout = 0);


    /// after the request is processed, the thread
    /// can be destroyed; returned to a pool, or other
    /// lifecycle strategy
    virtual void onExecutionFinish(TPRTask* task,Executor* executor,Request* request){
        taskFinish(task);
    }

protected:
    TPRTask* m_task;

    virtual TPRTask* getTask(){
        return m_task;
    }
    virtual void taskFinish(TPRTask* task){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)execution is finished\n")));
        return;
    }

};

#endif	/* _THREADPERREQUEST_H */

