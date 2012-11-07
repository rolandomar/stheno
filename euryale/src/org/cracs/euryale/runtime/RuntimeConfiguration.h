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
 * File:   Runtime.h
 * Author: rmartins
 *
 * Created on October 27, 2009, 3:31 PM
 */

#ifndef _RUNTIME_H
#define	_RUNTIME_H

#include <ace/Singleton.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>
#include <ace/Atomic_Op.h>
#include <ace/Synch_Traits.h>
#include <euryale/Types.h>
#include <ace/IOStream.h>
#include <sstream>
using std::stringstream;

class Interval {
public:

    Interval(int min, int max) :
    m_min(min), m_max(max) {
    }

    int getIntervalMin() {
        return m_min;
    }

    int getIntervalMax() {
        return m_max;
    }

    const stringstream& toString(stringstream& str){        
        str << "Interval(" << m_min << "," << m_max << ")";       
        return str;
    }

protected:
    int m_min;
    int m_max;
};

class RuntimeConfiguration {
public:

    enum {
        HRT = 0x1,
        SRT = 0x2,
        BE = 0x3
    } ExecutionType;

    // Best-effort max = min = priority = 0
    static const int BEPriority = 0;
    // Soft real-time
    static const int SRTMinPriority = 1;
    static const int SRTMaxPriority = 49;
    // Hard real-time
    static const int HRTMinPriority = 50;
    static const int HRTMaxPriority = 80; //99;

    Interval& getHRTPriorityInterval(){
        return *m_hrtInterval;
    }

    Interval& getSRTPriorityInterval(){
        return *m_srtInterval;
    }

    Interval& getBEPriorityInterval(){
        return *m_beInterval;
    }
   
    static RuntimeConfiguration* instance(void) {
        return ACE_Singleton<RuntimeConfiguration, ACE_SYNCH_RECURSIVE_MUTEX>::instance();
    }

    int getCPUCount(){
        return ACE_OS::num_processors();
    }
    
    static bool isClassDebugged(const char* className);

private:
    Interval* m_hrtInterval;
    Interval* m_srtInterval;
    Interval* m_beInterval;

    RuntimeConfiguration();
    virtual ~RuntimeConfiguration();
private:
    template <class, class> friend class ACE_Singleton;
    friend class
    ACE_Singleton <RuntimeConfiguration, ACE_SYNCH_RECURSIVE_MUTEX>;
};


#endif	/* _RUNTIME_H */

