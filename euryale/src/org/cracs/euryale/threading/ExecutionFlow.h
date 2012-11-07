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
 * ExecutionContext.h
 *
 *  Created on: Feb 11, 2008
 *      Author: rmartins
 */

#ifndef EXECUTIONCONTEXT_H_
#define EXECUTIONCONTEXT_H_


#include <ace/Task.h>
//#include <euryale/EuryaleConf.h>
#include <ace/OS_NS_Thread.h>
#include <euryale/Conf.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <euryale/event/Event.h>
#include <euryale/common/exception/Exception.h>

#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>

#include <euryale/threading/exception/ExecutionFlowException.h>
#include <euryale/threading/ExecutionFlowStack.h>
class ExecutionModel;

class ExecutionFlow : public ACE_TSS_Singleton<ExecutionFlow,
ACE_SYNCH_RECURSIVE_MUTEX>, /*public ExecutionFlowStack, */public Event {
public:
    ExecutionFlow(/*ExecutionModel* em = 0, Event* event = 0, ACE_Time_Value* timeout = 0*/);
    virtual ~ExecutionFlow();

    void push(ExecutionModel* em, Event* event = 0, ACE_Time_Value* timeout = 0)
        throw (ExecutionFlowException&);
    
    ExecutionFlowStack* pop();

    ExecutionFlowStack* current();

    ExecutionFlowStack* top();

    void setEvent(Event* event) throw (ExecutionFlowException&);

    /**
     * isUserContext() shows is a execution context belongs to
     * the userspace or if it was created by the framework (i.e
     * a executionModel)
     *
     * @return ...
     * @author rmartins
     */
    bool isUserFlow();

    ExecutionModel* getCurrentExecutionModel();

    ACE_Time_Value* getCurrentTimeout() ;

    ExecutionModel* getTopExecutionModel();

    ACE_Time_Value* getTopTimeout();

    bool isTimeout();
    

protected:    
    list<ExecutionFlowStack*> m_emStack;
};
#endif /* EXECUTIONCONTEXT_H_ */
