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
 * File:   ExecutionModelException.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 15, 2010, 10:44 AM
 */

#include "ExecutionModelException.h"


/*const char* ExecutionModelException::LOCK_ERROR = "Lock error";
const char* ExecutionModelException::BAD_BINDING = "Bad binding";
const char* ExecutionModelException::INITIALIZATION_ERROR = "Initialization error";
const char* ExecutionModelException::TIMEOUT = "Timeout";
const char* ExecutionModelException::JOIN_FAILED = "Join failed";*/

const char* ExecutionModelException::msgArray[] = {
    "Close error",
    "Lock error",
    "Bad binding",
    "Initialization error",
    "Timeout", "Join failed",
    "Error electing leader",
    "Error becoming leader",
    "No low level handler"
};

ExecutionModelException::ExecutionModelException(int type) {//const char* exception){
    m_type = type;
    m_messageString = new String(msgArray[type]);
}

ExecutionModelException::ExecutionModelException(int type, const char* exception) {//const char* exception){
    m_type = type;
    m_messageString = new String(exception);
}

ExecutionModelException::ExecutionModelException(const ExecutionModelException& ex) :
m_type(ex.m_type) {
    m_messageString = new String(*ex.m_messageString);
}

ExecutionModelException::~ExecutionModelException() {
    if (m_messageString != 0) {
        delete m_messageString;
    }
}