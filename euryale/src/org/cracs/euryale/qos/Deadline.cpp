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
 * File:   Deadline.cpp
 * Author: rmartins
 * 
 * Created on August 12, 2010, 4:31 PM
 */

#include <ace/OS_NS_sys_time.h>

#include "Deadline.h"

Deadline::Deadline(ACE_Time_Value* deadline) : m_deadline(deadline) {
    resetStart();
}

Deadline::Deadline(const Deadline& orig) {
    m_startTime = orig.m_startTime;
    m_deadline = new ACE_Time_Value(*orig.m_deadline);
}

Deadline::~Deadline() {
    if (m_deadline != 0) {
        delete m_deadline;
    }
}

void Deadline::resetStart() {

    m_startTime = ACE_OS::gettimeofday();
}

ACE_Time_Value* Deadline::getDeadline() {
    return m_deadline;
}

//caller must free

ACE_Time_Value* Deadline::getAbsoluteTime() {
    if (m_deadline == 0) {
        return 0; //INF
    }
    ACE_Time_Value* t = new ACE_Time_Value(m_startTime);
    (*t) += *m_deadline;
    return t;
}

//caller must free

ACE_Time_Value* Deadline::getRemainingDeadline() {
    return calculateRemainingTime();
}

ACE_Time_Value* Deadline::calculateRemainingTime() {
    if (m_deadline == 0) {
        return 0;
    }
    ACE_Time_Value now = ACE_OS::gettimeofday();
    ACE_Time_Value endTime(m_startTime);
    endTime += *m_deadline;
    if (now > endTime) {
        return new ACE_Time_Value(0, 0);
    }
    return new ACE_Time_Value((now - endTime));

}
