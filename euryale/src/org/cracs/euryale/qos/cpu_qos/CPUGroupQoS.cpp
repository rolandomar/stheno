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
 * File:   CPUGroupQoS.cpp
 * Author: rmartins
 * 
 * Created on December 18, 2009, 3:44 PM
 */

#include "CPUGroupQoS.h"

CPUGroupQoS::CPUGroupQoS() :
m_runtime(0), m_period(0), m_bestEffortWeigth(0) {
}

CPUGroupQoS::CPUGroupQoS(String cpuset, String schedDomain,UInt runtime, UInt period, UInt bestEffortWeigth) :
m_cpuSet(cpuset), m_cpuScheduleDomain(schedDomain),m_runtime(runtime), m_period(period), m_bestEffortWeigth(bestEffortWeigth) {
}

CPUGroupQoS::CPUGroupQoS(const CPUGroupQoS& orig) {
    m_cpuScheduleDomain = orig.m_cpuScheduleDomain;
    m_cpuSet = orig.m_cpuSet;
    m_runtime = orig.m_runtime;
    m_period = orig.m_period;
    m_bestEffortWeigth = orig.m_bestEffortWeigth;
}

CPUGroupQoS::~CPUGroupQoS() {
}

CPUGroupQoS& CPUGroupQoS::operator =(CPUGroupQoS const& qos) {
    m_cpuScheduleDomain = qos.m_cpuScheduleDomain;
    m_cpuSet = qos.m_cpuSet;
    m_runtime = qos.m_runtime;
    m_period = qos.m_period;
    m_bestEffortWeigth = qos.m_bestEffortWeigth;
    return *this;
}

void CPUGroupQoS::serialize(OutputStream& outputStream)
throw (SerializationException&) {    
    outputStream.write_string(m_cpuScheduleDomain);
    outputStream.write_string(m_cpuSet);
    outputStream.write_ulong(m_runtime);
    outputStream.write_ulong(m_period);
    outputStream.write_ulong(m_bestEffortWeigth);
}

void CPUGroupQoS::deserialize(InputStream& inputStream)
throw (SerializationException&) {    
    inputStream.read_string(m_cpuScheduleDomain);
    inputStream.read_string(m_cpuSet);
    inputStream.read_ulong(m_runtime);
    inputStream.read_ulong(m_period);
    inputStream.read_ulong(m_bestEffortWeigth);
}

UInt CPUGroupQoS::getRuntime() {
    return m_runtime;
}

UInt CPUGroupQoS::getPeriod() {
    return m_period;
}

UInt CPUGroupQoS::getBestEffortWeigth() {
    return m_bestEffortWeigth;
}

String& CPUGroupQoS::getCpusetDomain(){
    return m_cpuSet;
}

String& CPUGroupQoS::getCPUScheduleDomain(){
    return m_cpuScheduleDomain;
}
