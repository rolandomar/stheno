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
 * File:   Fault.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 3:24 PM
 */

#ifndef FAULT_H
#define	FAULT_H
#include <euryale/Types.h>
class Fault {
public:
    const static int RANDOM_LEVEL = INT_MAX;
    enum{
        CRASH_FAULT,
        TEMP_FAULT
    };
    Fault(int type,int level,ACE_Time_Value& startTime);
    Fault(const Fault& orig);
    virtual ~Fault();
    
    virtual String& toString() = 0;
    
    virtual int getLevel();
    virtual int getType();
    virtual ACE_Time_Value& getStartTime();    
    virtual bool isOverdue();
    
    virtual void sleepUntilDeadline();
    
protected:
    int m_type;
    //representation
    int m_level;    
    ACE_Time_Value m_startTime;
};

#endif	/* FAULT_H */

