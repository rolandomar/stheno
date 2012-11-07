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
 * File:   FaultFactory.h
 * Author: rmartins
 *
 * Created on February 8, 2011, 3:22 PM
 */

#ifndef FAULTFACTORY_H
#define	FAULTFACTORY_H
class Fault;
#include <stheno/core/p2p/p3/common/faults/CrashFault.h>
#include <stheno/core/p2p/p3/common/faults/TempFault.h>
class FaultFactory {
public:
    FaultFactory();
    FaultFactory(const FaultFactory& orig);
    virtual ~FaultFactory();
    
    static Fault* createFault(char* fault);
    
protected:
};

#endif	/* FAULTFACTORY_H */

