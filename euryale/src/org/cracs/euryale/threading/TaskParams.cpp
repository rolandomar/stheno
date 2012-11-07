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
 * File:   TaskParams.cpp
 * Author: rmartins
 * 
 * Created on December 20, 2010, 3:27 PM
 */

#include "TaskParams.h"

TaskParams::TaskParams(CPUQoS* cpuQoS, CPUReservation* qosReservation) :
m_cpuQoS(cpuQoS), m_qosReservation(qosReservation) {
}

TaskParams::TaskParams(const TaskParams& orig) :
m_cpuQoS(0), m_qosReservation(0) {
    if (orig.m_cpuQoS != 0) {
        m_cpuQoS = orig.m_cpuQoS->duplicate();
    }

    if (orig.m_qosReservation != 0) {
        m_qosReservation = orig.m_qosReservation->duplicate();
    }
}

TaskParams::~TaskParams() {
    if (m_cpuQoS != 0) {
        delete m_cpuQoS;
    }
    if (m_qosReservation != 0) {
        delete m_qosReservation;
    }
}

CPUQoS* TaskParams::getCPUQoS() {
    return m_cpuQoS;
}

CPUReservation* TaskParams::getCPUReservation() {
    return m_qosReservation;
}

TaskParams* TaskParams::duplicate() {
    return new TaskParams(*this);
}