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
 * File:   FaultFactory.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 3:22 PM
 */

#include "FaultFactory.h"
#include <stheno/core/p2p/p3/common/faults/Fault.h>

#include <ace/Tokenizer_T.h>
#include <euryale/common/Integer.h>

FaultFactory::FaultFactory() {
}

FaultFactory::FaultFactory(const FaultFactory& orig) {
}

FaultFactory::~FaultFactory() {
}

Fault* FaultFactory::createFault(char* fault){
    ACE_Tokenizer_T<char> tokenizer(fault);
    tokenizer.delimiter_replace (',', 0);
    const char* faultType = tokenizer.next ();
    if(ACE_OS::strcmp(faultType,"Crash")==0){        
        char* st = tokenizer.next ();
        int level = Integer::parseInt(st,10);        
        st = tokenizer.next ();
        int sec = Integer::parseInt(st,10);        
        ACE_Time_Value startTime(sec,0);
        CrashFault* cf = new CrashFault(level,startTime);
        return cf;        
    }
    if(ACE_OS::strcmp(faultType,"Temp")==0){        
        char* st = tokenizer.next ();
        int level = Integer::parseInt(st,10);        
        st = tokenizer.next ();
        int sec = Integer::parseInt(st,10);
        ACE_Time_Value startTime(sec,0);
        st = tokenizer.next ();
        sec = Integer::parseInt(st,10);
        ACE_Time_Value endTime(sec,0);
        TempFault* tf = new TempFault(level,startTime,endTime);
        return tf;
    }
    return 0;
}