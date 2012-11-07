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
 * File:   Backtrace.cpp
 * Author: rmartins
 * 
 * Created on August 12, 2010, 6:28 PM
 */

#include "Backtrace.h"

Backtrace::Backtrace() {
}

Backtrace::~Backtrace() {
}

void Backtrace::backstrace() {
    void *array[MAX_BACKTRACE_DEPTH];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, MAX_BACKTRACE_DEPTH);
    //write to Error output
    backtrace_symbols_fd(array, size, 2);
}