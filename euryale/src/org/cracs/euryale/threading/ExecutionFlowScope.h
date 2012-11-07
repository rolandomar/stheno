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
 * File:   ExecutionFlowScope.h
 * Author: rmartins
 *
 * Created on January 10, 2011, 6:21 PM
 */

#ifndef EXECUTIONFLOWSCOPE_H
#define	EXECUTIONFLOWSCOPE_H

#include <euryale/event/Event.h>
#include <euryale/threading/ExecutionModel.h>
#include <euryale/threading/exception/ExecutionFlowException.h>
#include <euryale/threading/ExecutionFlow.h>

class ExecutionFlowScope {
public:

    ExecutionFlowScope(ExecutionModel* em, Event* event = 0, ACE_Time_Value* timeout = 0)
        throw (ExecutionFlowException&);
    
    virtual void setEvent(Event* event) throw (ExecutionFlowException&);
    virtual ~ExecutionFlowScope();
protected:
    ExecutionFlow* m_ec;
};

#endif	/* EXECUTIONFLOWSCOPE_H */

