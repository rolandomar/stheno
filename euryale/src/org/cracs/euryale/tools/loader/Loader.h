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
 * File:   Loader.h
 * Author: rmartins
 *
 * Created on February 11, 2011, 10:56 AM
 */

#ifndef LOADER_H
#define	LOADER_H

#include <ace/Time_Value.h>
#include <euryale/threading/Task.h>

class Loader: public Task {
public:
  static int const MAXBURNPERIOD  = 999999;//50000
    //static int const MAXBURNPERIOD  = 10;//50000
    Loader();
    virtual ~Loader();
    
    int open(int load,
        int threads = ACE_OS::num_processors(),
        ACE_Time_Value* timeout = 0,
        CPUReservation* reserve = 0,
        CPUQoS* qos = 0,bool verbose = false);    
    
    virtual int svc_i(void* arg);
    
    void close(bool waitForFinish);
    
protected:
    ACE_Time_Value* m_timeout;
    bool m_open;
    bool m_closed;
    int m_load;
    int m_threads;
    bool m_verbose;    
    
    virtual int close (u_long flags = 0){return 0;}
    bool isActive();
    ACE_Time_Value* calculatePeriod(int load,int threads);
    ACE_Time_Value* calculatePeriodWithVariableTimeUnit(int load, int timeUnit);

};

#endif	/* LOADER_H */

