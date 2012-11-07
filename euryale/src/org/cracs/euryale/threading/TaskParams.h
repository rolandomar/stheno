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
 * File:   TaskParams.h
 * Author: rmartins
 *
 * Created on December 20, 2010, 3:27 PM
 */

#ifndef TASKPARAMS_H
#define	TASKPARAMS_H

#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/threading/exception/TaskException.h>


class TaskParams {
public:
    TaskParams(CPUQoS* cpuQoS,CPUReservation* qosReservation);
    TaskParams(const TaskParams& orig);
    virtual ~TaskParams();
    
    CPUQoS* getCPUQoS();
    CPUReservation* getCPUReservation();
    
    virtual TaskParams* duplicate();
    
protected:
    CPUQoS* m_cpuQoS;
    CPUReservation* m_qosReservation;

};

#endif	/* TASKPARAMS_H */

