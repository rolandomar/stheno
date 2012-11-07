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
 * File:   Fault.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 3:24 PM
 */

#include "Fault.h"
#include <euryale/common/sleep/SleepHelper.h>

Fault::Fault(int type, int level,ACE_Time_Value& startTime) :
m_type(type), m_level(level),m_startTime(startTime) {
}

Fault::Fault(const Fault& orig) :
m_type(orig.m_type), m_level(orig.m_level),m_startTime(orig.m_startTime) {
}

Fault::~Fault() {
}

int Fault::getLevel() {
    return m_level;
}

int Fault::getType() {
    return m_type;
}

ACE_Time_Value& Fault::getStartTime(){
    return m_startTime;
}

bool Fault::isOverdue(){
    ACE_Time_Value now = ACE_OS::gettimeofday();
    return( now >= m_startTime);
}

void Fault::sleepUntilDeadline(){
    if(isOverdue()){
        return;
    }
    ACE_Time_Value startTime = m_startTime;
    startTime -= ACE_OS::gettimeofday();
    SleepHelper::sleep(startTime);        
}