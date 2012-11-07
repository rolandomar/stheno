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
 * File:   ResourceMonitor.cpp
 * Author: rmartins
 * 
 * Created on February 10, 2011, 2:31 PM
 */

#include "ResourceMonitor.h"
#include <euryale/common/sleep/SleepHelper.h>

ResourceMonitor::ResourceMonitor(ACE_Time_Value* sleepTime) :
m_closed(false), m_started(false), m_outputStream(0), m_sleepTime(0, 250000) {
    if (sleepTime != 0) {
        m_sleepTime = *sleepTime;
        delete sleepTime;
    }
}

ResourceMonitor::~ResourceMonitor() {
    shutdown();
}

void ResourceMonitor::shutdown() {
    if (m_closed) {
        return;
    }
    m_closed = true;
    this->wait();
    if (m_outputStream != 0) {
        delete m_outputStream;
        m_outputStream = 0;
    }
}

int ResourceMonitor::open(const char* filename) {
    if (m_started) {
        return -1;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    if (filename != 0) {
        m_outputStream = new std::ofstream(filename);
    }
    fetchTotalMemory();
    return this->activate();
}

int ResourceMonitor::open(String& filename) {
    if (m_started) {
        return -1;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    m_outputStream = new std::ofstream(m_filename.c_str());
    fetchTotalMemory();
    return this->activate();
}

ACE_OSTREAM_TYPE* ResourceMonitor::getOutputStream() {
    return m_outputStream;
}

void ResourceMonitor::logLine(const char* line) {
    if (m_outputStream == 0) {
        return;
    }
    (*m_outputStream) << line;
    m_outputStream->flush();
}

void ResourceMonitor::logLine(String& line) {
    if (m_outputStream == 0) {
        return;
    }
    (*m_outputStream) << line;
    m_outputStream->flush();
}

int ResourceMonitor::svc_i(void* arg) {
    double ototal, total;
    ACE_UINT64 u;
    double ovm, orss, outime, ostime;
    double vm, rss, utime, stime;    
    ACE_Time_Value oNow = ACE_OS::gettimeofday();
    process_mem_usage(ovm, orss, outime, ostime);
    while (!m_closed) {        
        SleepHelper::sleep(m_sleepTime);
        process_mem_usage(vm, rss, utime, stime);
        ACE_Time_Value now = ACE_OS::gettimeofday();
        ACE_Time_Value delta = now - oNow;

        total = (utime + stime);
        ototal = (outime + ostime);
        delta.to_usec(u);
        double per = ((double) (total - ototal) / (double) u)*100000000;
        
        //printf("ototal=%f total=%f diff=%f delta=%lu mem=%ly\n",ototal,total,(total-ototal),u,m_mem);
        if (per < 0) {
            per *= -1;
        }
        
        total = (vm + rss);
        //ototal = (ovm + orss);
        u = m_mem;
        double perMem = ((double) (total) / (double) (u*1024))*100;
        
        stringstream ss;
        ss << "(" << now << ")" << " VM(" << vm << ") RSS(" << rss << ") "
                "UTIME(" << utime << ") STIME(" << stime << ") PCPU(" << per 
                << ") PMEM(" << perMem << ")"                
                << endl;        
        if (m_outputStream != 0) {
            logLine(ss.str().c_str());
        }else{
            printf("%s\n",ss.str().c_str());
        }
        ovm = vm;
        orss = rss;
        outime = utime;
        ostime = stime;
        oNow = now;
    }
    return 0;
}


void ResourceMonitor::fetchTotalMemory(){
    using std::ios_base;
    using std::ifstream;
    using std::string;
    //MemTotal:        6116724 kB
    string a,b,c;    
    ifstream stat_stream("/proc/meminfo", ios_base::in);
    stat_stream >> a >> m_mem >> c;    
}

void ResourceMonitor::process_mem_usage(double& vm_usage, double& resident_set, double& utime_usage, double& stime_usage) {
    using std::ios_base;
    using std::ifstream;
    using std::string;


    vm_usage = 0.0;
    resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    //string utime, stime, cutime, cstime, priority, nice;
    string priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;
    unsigned long utime, stime, cutime, cstime;

    /*string rsslim, startcode, endcode, startstack, kstkesp, kstkeip,
            signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap,
            exit_signal, processor, rt_priority, policy, delayacct_blkio_ticks;


    unsigned long guest_time;
    long cguest_time;*/

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; /* >>
            rsslim >> startcode >> endcode >> startstack >> kstkesp >> kstkeip >>
            signal>> blocked >> sigignore >> sigcatch >> wchan >> nswap >> cnswap >>
            exit_signal >> processor >> rt_priority >> policy >> delayacct_blkio_ticks >>
            guest_time >> cguest_time;*/

    //printf("utime(%s) stime(%s)\n", utime.c_str(), stime.c_str());
    utime_usage = utime + cutime;
    if (utime_usage > 0) {
        utime_usage = utime_usage / HZ;
    }
    stime_usage = stime + cstime;
    if (stime_usage > 0) {
        stime_usage = stime_usage / HZ;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}
