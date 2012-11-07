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
 * File:   SleepHelper.cpp
 * Author: rmartins
 * 
 * Created on May 29, 2010, 4:28 PM
 */

#include <ace/OS_NS_unistd.h>
#include <ace/Log_Msg.h>
#include "SleepHelper.h"


SleepHelper::SleepHelper() {
}

SleepHelper::~SleepHelper() {
}

void SleepHelper::sleep(ACE_Message_Block& sleepTime){
    SleepHelper::sleep(sleepTime);    
}

void SleepHelper::sleep(time_t sec, suseconds_t usec){
    ACE_Time_Value sleepTime(sec,usec);
    SleepHelper::sleep(sleepTime);    
}

void SleepHelper::sleep(ACE_Time_Value& sleepTime){
    ACE_OS::sleep(sleepTime);
}

void SleepHelper::sleepRandom(ACE_Time_Value& maxTime){
    ACE_Time_Value t;
    if(maxTime.sec() == 0){
        t.sec(0);
    }else{
        t.sec(ACE_OS::rand()%maxTime.sec());
    }
    if(maxTime.usec() == 0){
        t.usec(0);
    }else{
        t.usec(ACE_OS::rand()%maxTime.usec());
    }
    SleepHelper::sleep(t);
}

void SleepHelper::sleep(int msec){
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Sleeping for %d msec\n",1000);
    ACE_Time_Value t(0,convertMsecToUsec(msec));    
    SleepHelper::sleep(t);
}

void SleepHelper::sleepRandom(int msec){
    UInt ms = ACE_OS::rand()%convertMsecToUsec(msec);
    ACE_Time_Value t(0,ms);    
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: SleepHelper::sleepRandom() - sleeping for %d msec\n"),ms/1000));
    SleepHelper::sleep(t);
}