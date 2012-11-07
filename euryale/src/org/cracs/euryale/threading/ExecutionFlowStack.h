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
 * File:   ExecutionFlowStack.h
 * Author: rmartins
 *
 * Created on January 10, 2011, 6:14 PM
 */

#ifndef EXECUTIONFLOWSTACK_H
#define	EXECUTIONFLOWSTACK_H

#include <euryale/event/Event.h>
#include <euryale/threading/exception/ExecutionFlowException.h>
class ExecutionModel;
class ExecutionFlowStack {
public:

    ExecutionFlowStack();
    ExecutionFlowStack(ExecutionModel* em);
    ExecutionFlowStack(ExecutionModel* em, Event* event = 0, ACE_Time_Value* timeout = 0);
    virtual ~ExecutionFlowStack();

    bool hasTimeout();
    Event* getEvent();
    ExecutionModel* getExecutionModel();
    ACE_Time_Value* getTimeout();
    bool isUserContext();
    void setEvent(Event* event) throw (ExecutionFlowException&);

protected:
    ACE_Time_Value* m_timeout;
    Event* m_event;
    ExecutionModel* m_em;
};


#endif	/* EXECUTIONFLOWSTACK_H */

