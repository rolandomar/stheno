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
 * ExecutionContext.cpp
 *
 *  Created on: Feb 11, 2008
 *      Author: rmartins
 */

#include "ExecutionFlow.h"
#include "ExecutionModel.h"

ExecutionFlow::ExecutionFlow(/*ExecutionModel* em, Event* event,
        ACE_Time_Value* timeout*/) {
    //push(em, event, timeout);

}

ExecutionFlow::~ExecutionFlow() {
    //ExecutionFlowStack* stack = pop();
    //delete stack;
}

void ExecutionFlow::push(ExecutionModel* em, Event* event, ACE_Time_Value* timeout)
throw (ExecutionFlowException&) {
    if (timeout == 0) {
        ACE_Time_Value* currentTimeout = getCurrentTimeout();
        if (currentTimeout != 0) {
            timeout = new ACE_Time_Value(*currentTimeout);
        }
    } else {
        ACE_Time_Value* currentTimeout = getCurrentTimeout();
        if (currentTimeout != 0) {
            if (timeout >= currentTimeout) {
                throw ExecutionFlowException(ExecutionFlowException::EXECUTION_TIMEOUT_EXCEDEED);
            }
        }
    }
    m_emStack.push_back(new ExecutionFlowStack(em, event, timeout));
}

ExecutionFlowStack* ExecutionFlow::pop() {
    if (m_emStack.size() == 0) {
        return 0;
    }
    ExecutionFlowStack* ret = m_emStack.back();
    m_emStack.erase(m_emStack.end());
    return ret;
}

ExecutionFlowStack* ExecutionFlow::current() {
    ExecutionFlowStack* ret = m_emStack.back();
    return ret;
}

ExecutionFlowStack* ExecutionFlow::top() {
    ExecutionFlowStack* ret = m_emStack.front();
    return ret;
}

void ExecutionFlow::setEvent(Event* event)
throw (ExecutionFlowException&) {
    ExecutionFlowStack* c = current();
    c->setEvent(event);
}

/**
 * isUserContext() shows is a execution context belongs to
 * the userspace or if it was created by the framework (i.e
 * a executionModel)
 *
 * @return ...
 * @author rmartins
 */
bool ExecutionFlow::isUserFlow() {
    return m_emStack.front()->isUserContext();
}

ExecutionModel* ExecutionFlow::getCurrentExecutionModel() {
    printf("===================>%ld\n",m_emStack.size());
    ExecutionFlowStack* ret = m_emStack.back();
    if (ret == 0) {
        return 0;
    }
    return ret->getExecutionModel();
}

ACE_Time_Value* ExecutionFlow::getCurrentTimeout() {
    ExecutionFlowStack* ret = m_emStack.back();
    if (ret == 0) {
        return 0;
    }
    return ret->getTimeout();
}

ExecutionModel* ExecutionFlow::getTopExecutionModel() {
    ExecutionFlowStack* ret = m_emStack.front();
    if (ret == 0) {
        return 0;
    }
    return ret->getExecutionModel();
}

ACE_Time_Value* ExecutionFlow::getTopTimeout() {
    ExecutionFlowStack* ret = m_emStack.front();
    if (ret == 0) {
        return 0;
    }
    return ret->getTimeout();
}

bool ExecutionFlow::isTimeout() {
    ACE_Time_Value* timeout = getCurrentTimeout();
    if (timeout == 0) {
        return false;
    }
    ACE_Time_Value now = ACE_OS::gettimeofday();
    return (now >= *timeout);
}
