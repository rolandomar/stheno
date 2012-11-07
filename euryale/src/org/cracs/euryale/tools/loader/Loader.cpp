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
 * File:   Loader.cpp
 * Author: rmartins
 * 
 * Created on February 11, 2011, 10:56 AM
 */

#include "Loader.h"
#include <euryale/common/sleep/SleepHelper.h>

Loader::Loader() :
m_load(0), m_threads(0), m_open(false), m_closed(false), m_timeout(0), m_verbose(false) {
}

Loader::~Loader() {
    if (m_timeout != 0) {
        delete m_timeout;
    }
}

void Loader::close(bool waitForFinish) {
    if (waitForFinish && m_timeout != 0) {
        this->wait();
    }
    m_closed = true;
    if (waitForFinish && m_timeout == 0) {
        this->wait();
    }
}

int Loader::open(int load, int threads, ACE_Time_Value* timeout, CPUReservation* reserve, CPUQoS* qos, bool verbose) {
    if (m_open) {
        if (m_verbose) {
            printf("Loader: already started!\n");
        }
        if (timeout != 0)
            delete timeout;
        return -1;
    }
    m_verbose = verbose;
    if (load < 0 || load > 100) {
        if (m_verbose) {
            printf("Loader: load must be >0 and <100!\n");
        }
        if (timeout != 0)
            delete timeout;
        return -1;
    }
    m_open = true;
    m_load = load;
    m_threads = threads;
    /*if(m_threads > 4){
        m_threads = 4;
    }*/
    if (timeout != 0) {
        *timeout += ACE_OS::gettimeofday();
        m_timeout = timeout;
    }else{
        m_timeout = 0;
    }
    long flags = THR_NEW_LWP | THR_JOINABLE;// | THR_INHERIT_SCHED;
    
    return this->activate(reserve, qos, flags, m_threads);
}

bool Loader::isActive() {
    if (m_closed) {
        return false;
    }
    if (m_timeout != 0) {        
        if (ACE_OS::gettimeofday() > *m_timeout) {
            return false;
        }
    }
    return true;
}

ACE_Time_Value* Loader::calculatePeriod(int load, int threads) {
    float per = load / 100.0;
    float share = (float) 1.0; //(float)ACE_OS::num_processors()/(float)threads;
    float sleepTime = (per * 1000000) * share;
    ACE_Time_Value* p = new ACE_Time_Value(0, sleepTime);
    return p;
}

ACE_Time_Value* Loader::calculatePeriodWithVariableTimeUnit(int load, int timeUnit) {
    float per = load / 100.0;
    float share = (float) 1.0; //(float)ACE_OS::num_processors()/(float)threads;
    float sleepTime = (per * timeUnit) * share;
    ACE_Time_Value* p = new ACE_Time_Value(0, sleepTime);
    //ACE_Time_Value* p = new ACE_Time_Value(sleepTime,0);
    return p;
}

int Loader::svc_i(void* arg) {
    ACE_Time_Value* p = calculatePeriod(m_load, m_threads);
    ACE_Time_Value sleepTime(1, 0);
    sleepTime -= *p;
    if (m_verbose) {
        printf("Loader: Thread started(%d) with sleep(%ld)\n", gettid(), p->msec());
    }
    ULongLong counter = 0;
    ULongLong innerCounter = 0;
    while (isActive()) {
        ACE_Time_Value start = ACE_OS::gettimeofday();
        suseconds_t timeUnit = ACE_OS::rand()%MAXBURNPERIOD;
        sleepTime = ACE_Time_Value(0,timeUnit);
        //time_t timeUnit = ACE_OS::rand()%MAXBURNPERIOD;
        //sleepTime = ACE_Time_Value(timeUnit,0);
        ACE_Time_Value* p = calculatePeriodWithVariableTimeUnit(m_load, timeUnit);
        ACE_Time_Value deadline = ACE_OS::gettimeofday();        
        sleepTime -= *p;
        deadline += (*p);
        
        float crunch = 29387213.23;
        innerCounter = 0;
        while (ACE_OS::gettimeofday() < deadline && isActive()) {            
            //for(int i=0; i < 100000; i++){
                crunch *= crunch * (crunch / 23987);
                crunch /= (3209871);
                innerCounter++;
            //}
        }
        counter++;
        if (m_verbose) {
            ACE_Time_Value end = ACE_OS::gettimeofday() - start;
            ULongLong usec;
            end.to_usec(usec);
            printf("Loader: counter(%ld) innerCounter(%ld) time_elapsed(%ld) next_sleep(%ld) period(%ld)\n",
                    counter,innerCounter,end.msec(),sleepTime.msec(),(*p).msec());
        }
        SleepHelper::sleep(sleepTime);        
        delete p;
                
    }
    
    /*while (isActive()) {
        ACE_Time_Value deadline = ACE_OS::gettimeofday();
        deadline += (*p);
        float crunch = 29387213.23;
        while (ACE_OS::gettimeofday() < deadline && isActive()) {            
            for(int i=0; i < 100000; i++){
                crunch *= crunch * (crunch / 23987);
                crunch /= (3209871);
            }
        }
        SleepHelper::sleep(sleepTime);        
    }*/
    if (m_verbose) {
        printf("Loader: Thread finished(%d)\n", gettid());
    }

    delete p;
    return 0;
}
