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
 * File:   IncrementalSleep.cpp
 * Author: rmartins
 * 
 * Created on January 31, 2011, 3:30 PM
 */

#include "IncrementalSleep.h"

IncrementalSleep::IncrementalSleep(time_t sec, suseconds_t usec):
m_incrTime(sec,usec),m_current(m_incrTime){    
}

IncrementalSleep::IncrementalSleep(ACE_Time_Value& incrementalSleepTime):
m_incrTime(incrementalSleepTime),m_current(incrementalSleepTime)
{
}

IncrementalSleep::IncrementalSleep(const IncrementalSleep& orig):
m_incrTime(orig.m_incrTime),m_current(orig.m_current)
{
}

IncrementalSleep::~IncrementalSleep() {
}

void IncrementalSleep::sleep(){
    SleepHelper::sleep(m_current);
    m_current += m_incrTime;   
}