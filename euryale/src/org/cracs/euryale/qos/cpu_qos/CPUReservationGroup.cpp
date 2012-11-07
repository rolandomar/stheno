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
 * File:   CPUReservationGroup.cpp
 * Author: rmartins
 * 
 * Created on November 27, 2009, 3:29 PM
 */

#include "CPUReservationGroup.h"

CPUReservationGroup::CPUReservationGroup() {
}

/*CPUReservationGroup::CPUReservationGroup(const CPUReservationGroup& orig) {
}*/

CPUReservationGroup::~CPUReservationGroup() {
    close();
}

void CPUReservationGroup::open(string& name, CPUGroupQoS& qos) throw (CgroupException&) {
    m_cpuGroupQoS = qos;
    m_name = name;
}

void CPUReservationGroup::close() {

}

string& CPUReservationGroup::getName() {
    return m_name;
}

CPUGroupQoS& CPUReservationGroup::getGroupQoS() {
    return m_cpuGroupQoS;
}