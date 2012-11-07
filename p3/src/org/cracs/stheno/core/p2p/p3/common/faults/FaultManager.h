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
 * File:   FaultManager.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 12:44 PM
 */

#ifndef FAULTMANAGER_H
#define	FAULTMANAGER_H

#include <euryale/Types.h>
#include <euryale/threading/Task.h>
#include <list>
using std::list;

class Fault;
class Stheno;
class FaultManager: public Task {
public:
    const static int MAX_BUFFER_SIZE = 2048;
    FaultManager(Stheno* runtime);    
    virtual ~FaultManager();
    
    bool attachFaults(const char* faults);

    int open(int numCrashFaults,int numTempFaults,int delay = 0);
    
    virtual int svc_i(void* arg);
    
protected:  
    Stheno* m_runtime;
    list<Fault*> m_faultList;
    char m_faults[MAX_BUFFER_SIZE];
    bool parseFaults(const char* faults);
    bool parseFault(char* fault);
    
    void generateFaults(int crashFaults, int tempFaults);
    virtual void performFault(Fault* f);
        
};

#endif	/* FAULTMANAGER_H */

