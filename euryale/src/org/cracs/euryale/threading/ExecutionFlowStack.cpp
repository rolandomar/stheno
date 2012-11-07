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
 * File:   ExecutionFlowStack.cpp
 * Author: rmartins
 * 
 * Created on January 10, 2011, 6:14 PM
 */

#include "ExecutionFlowStack.h"

ExecutionFlowStack::ExecutionFlowStack():
m_em(0),m_timeout(0),m_event(0)
{}

ExecutionFlowStack::ExecutionFlowStack(ExecutionModel* em):
m_em(em),m_timeout(0),m_event(0)
{}

ExecutionFlowStack::ExecutionFlowStack(ExecutionModel* em, Event* event, ACE_Time_Value* timeout):
m_em(em),m_timeout(timeout),m_event(event)
{}

ExecutionFlowStack::~ExecutionFlowStack() {
    if (m_timeout != 0) {
        delete m_timeout;
    }
    if (m_event != 0) {
        delete m_event;
    }
}

bool ExecutionFlowStack::hasTimeout() {
    return (m_timeout != 0);
}

Event* ExecutionFlowStack::getEvent() {
    return m_event;
}

ExecutionModel* ExecutionFlowStack::getExecutionModel() {
    return m_em;
}

ACE_Time_Value* ExecutionFlowStack::getTimeout() {
    return m_timeout;
}

bool ExecutionFlowStack::isUserContext() {
    return (m_em == 0);
}

void ExecutionFlowStack::setEvent(Event* event) throw (ExecutionFlowException&) {
    if (m_event == 0) {
        m_event = event;
    } else {
        throw (new ExecutionFlowException(ExecutionFlowException::SYSCALL_FAILED));
    }
}