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
 * File:   Deadline.h
 * Author: rmartins
 *
 * Created on August 12, 2010, 4:31 PM
 */

#ifndef DEADLINE_H
#define	DEADLINE_H

#include <ace/Time_Value.h>

class Deadline {
public:

    Deadline(ACE_Time_Value* deadline);
    Deadline(const Deadline& orig);
    virtual ~Deadline();
    //
    void resetStart();
    //
    ACE_Time_Value* getDeadline();
    //caller must free
    ACE_Time_Value* getAbsoluteTime();
    //caller must free
    ACE_Time_Value* getRemainingDeadline();

protected:
    ACE_Time_Value* m_deadline;
    ACE_Time_Value m_startTime;

    ACE_Time_Value* calculateRemainingTime() ;
};

#endif	/* DEADLINE_H */

