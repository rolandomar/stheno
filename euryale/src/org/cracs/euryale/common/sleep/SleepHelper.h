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
 * File:   SleepHelper.h
 * Author: rmartins
 *
 * Created on May 29, 2010, 4:28 PM
 */

#ifndef SLEEPHELPER_H
#define	SLEEPHELPER_H

#include <ace/Time_Value.h>
#include <euryale/Types.h>

class SleepHelper {
public:
    SleepHelper();   
    virtual ~SleepHelper();

    static void sleep(ACE_Time_Value& sleepTime);
    static void sleep(time_t sec, suseconds_t usec = 0);
    static void sleep(ACE_Message_Block& sleepTime);
    static void sleep(int msec);

    static void sleepRandom(ACE_Time_Value& maxTime);
    static void sleepRandom(int msec);


private:
    static int convertMsecToUsec(int msec){
        return msec*1000;
    }
};

#endif	/* SLEEPHELPER_H */

