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
 * File:   CPUReservation.h
 * Author: rmartins
 *
 * Created on November 3, 2009, 5:48 PM
 */

#ifndef _CPURESERVATION_H
#define	_CPURESERVATION_H

#include <ace/OS_NS_Thread.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

class CPUReservationGroup;
class CPUQoS;
class CPUReservation {
public:    
  static const int DEFAULT_PERIOD =  10000;//100;//1000000;
  static const int ROOT_RUNTIME_US = 10000;//100;//950000;
  static const int BE_RESERVATION =  9000;//90;//700000;
    
    CPUReservation(String& cpusetDomain,String& scheduleDomain,CPUQoS* qos);
    CPUReservation(const CPUReservation& orig);
    virtual ~CPUReservation();

    virtual CPUReservation* duplicate();
    
    String& getCpusetDomain();
    String& getScheduleDomain();    
    CPUQoS* getQoS();
protected:
    String m_cpusetDomain;
    String m_nullCpusetDomain;
    String m_scheduleDomain;
    CPUQoS* m_cpuQos;
};

#endif	/* _CPURESERVATION_H */

