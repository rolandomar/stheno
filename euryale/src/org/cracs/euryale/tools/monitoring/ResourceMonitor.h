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
 * File:   ResourceMonitor.h
 * Author: rmartins
 *
 * Created on February 10, 2011, 2:31 PM
 */

#ifndef RESOURCEMONITOR_H
#define	RESOURCEMONITOR_H
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include <ace/Log_Msg.h>

#include <euryale/threading/Task.h>
#include <sstream>
using std::stringstream;

class ResourceMonitor : Task {
public:
    ResourceMonitor(ACE_Time_Value* sleepTime = 0);
    virtual ~ResourceMonitor();
    
    void process_mem_usage(double& vm_usage, double& resident_set,double& utime_usage, double& stime_usage);

    virtual int svc_i(void* arg);
   
    ACE_OSTREAM_TYPE* getOutputStream();
    void logLine(String& line);
    void logLine(const char* line);
    int open(String& filename);
    int open(const char* filename);
    void shutdown();
protected:
    String m_filename;
    bool m_started;
    bool m_closed;
    ACE_OSTREAM_TYPE *m_outputStream;
    ACE_Time_Value m_sleepTime;
    unsigned long m_mem;

    //virtual int close (u_long flags = 0){return 0;}
    void fetchTotalMemory();
    
    
    virtual int close (u_long flags = 0){return 0;}
};

#endif	/* RESOURCEMONITOR_H */

