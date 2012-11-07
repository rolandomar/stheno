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
 * File:   CPUReservation.cpp
 * Author: rmartins
 * 
 * Created on November 3, 2009, 5:48 PM
 */

#include "CPUReservation.h"
#include "CPUReservationGroup.h"
#include "CPUQoS.h"
#include <euryale/qos/medusa/Cgroup.h>

CPUReservation::CPUReservation(String& cpusetDomain, String& scheduleDomain, CPUQoS* qos) :
m_cpusetDomain(cpusetDomain), m_scheduleDomain(scheduleDomain), m_cpuQos(qos)
 {
}

CPUReservation::CPUReservation(const CPUReservation& orig) :
m_cpusetDomain(orig.m_cpusetDomain), m_scheduleDomain(orig.m_scheduleDomain),
m_cpuQos(orig.m_cpuQos->duplicate()) {
}

CPUReservation::~CPUReservation() {
    delete m_cpuQos;
}

CPUQoS* CPUReservation::getQoS() {
    return m_cpuQos;
}

String& CPUReservation::getCpusetDomain() {
    if(Cgroup::PARTITIONING){
        return m_cpusetDomain;
    }else{
        return m_nullCpusetDomain;
    }
}

String& CPUReservation::getScheduleDomain() {
    return m_scheduleDomain;
}

CPUReservation* CPUReservation::duplicate() {
    return new CPUReservation(*this);
}
