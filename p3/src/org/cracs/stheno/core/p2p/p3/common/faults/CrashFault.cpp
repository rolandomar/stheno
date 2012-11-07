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
 * File:   CrashFault.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 3:16 PM
 */

#include "CrashFault.h"

CrashFault::CrashFault(int level,ACE_Time_Value& startTime):
Fault(Fault::CRASH_FAULT,level,startTime)
{
}

CrashFault::CrashFault(const CrashFault& orig):
Fault(orig)
{
}

CrashFault::~CrashFault() {
}

