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
 * File:   IncrementalSleep.h
 * Author: rmartins
 *
 * Created on January 31, 2011, 3:30 PM
 */

#ifndef INCREMENTALSLEEP_H
#define	INCREMENTALSLEEP_H

#include <ace/Time_Value.h>
#include <euryale/common/sleep/SleepHelper.h>

class IncrementalSleep: public SleepHelper {
public:
    
    IncrementalSleep(time_t sec, suseconds_t usec = 0);
    IncrementalSleep(ACE_Time_Value& incrementalSleepTime);
    IncrementalSleep(const IncrementalSleep& orig);
    virtual ~IncrementalSleep();
    
    void sleep();
    
protected:
    ACE_Time_Value m_incrTime;
    ACE_Time_Value m_current;

};

#endif	/* INCREMENTALSLEEP_H */

