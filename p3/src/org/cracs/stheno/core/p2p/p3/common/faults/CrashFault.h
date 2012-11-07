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
 * File:   CrashFault.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 3:16 PM
 */

#ifndef CRASHFAULT_H
#define	CRASHFAULT_H

#include <stheno/core/p2p/p3/common/faults/Fault.h>
#include <ace/Time_Value.h>
#include <euryale/common/Integer.h>
#include <sstream>
using std::stringstream;

class CrashFault: public Fault {
public:
    CrashFault(int level,ACE_Time_Value& startTime);
    CrashFault(const CrashFault& orig);
    virtual ~CrashFault();
    
    virtual String& toString(){        
        stringstream ss;
        ss << "CrashFault(" << m_level << "," << Integer::toUnsignedString(m_startTime.sec(),10) << ")";
        m_str = ss.str().c_str();
        return m_str;
    }
protected:
    String m_str;    

};

#endif	/* CRASHFAULT_H */

