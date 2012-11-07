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
 * File:   ExecutionFlowException.cpp
 * Author: rmartins
 * 
 * Created on January 10, 2011, 6:09 PM
 */

#include "ExecutionFlowException.h"

const char* ExecutionFlowException::ExecutionContextExceptionMessage[] = {
    "Resource denied.",
    "No resources available.",
    "Execution has timed out.",
    "Syscall not implemented.",
    "Syscall failed.",
    "Timeout excedeed."

};

ExecutionFlowException::ExecutionFlowException(int type) :
m_type(type) {
}

ExecutionFlowException::~ExecutionFlowException() {
}

const String& ExecutionFlowException::message() {
    if (m_messageString == 0) {
        m_messageString
                = new String(ExecutionContextExceptionMessage[m_type]);
    }
    return *m_messageString;
}

const char* ExecutionFlowException::objectID() const {
    return "aff3860431acd3efc877ad8c0bf23e78";
}

const String& ExecutionFlowException::toString() {
    if (m_toString == 0)
        m_toString = new String("ExecutionContextException");
    return *m_toString;
}