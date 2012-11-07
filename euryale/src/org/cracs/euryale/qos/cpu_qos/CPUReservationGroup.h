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
 * File:   CPUReservationGroup.h
 * Author: rmartins
 *
 * Created on November 27, 2009, 3:29 PM
 */

#ifndef _CPURESERVATIONGROUP_H
#define	_CPURESERVATIONGROUP_H
///system
#include <string>
using std::string;
#include <list>
using std::list;
///ACE
#include <ace/OS_NS_Thread.h>
///stheno
#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/ReservationGroup.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUGroupQoS.h>

#include <euryale/Types.h>

class CPUReservationGroup : public ReservationGroup {
public:
    CPUReservationGroup();
    ~CPUReservationGroup();

    void open(string& name, CPUGroupQoS& qos) throw (CgroupException&);
    void close();   
    //Get path of this group
    virtual string& getName();
    virtual CPUGroupQoS& getGroupQoS();

    
    virtual list<ACE_id_t>* getTasks() 
        throw (CgroupException&) = 0;
    //reserve task
    virtual CPUReservation* reserveCPU(ACE_id_t pid, CPUQoS& qos) 
        throw (CgroupException&) = 0;
    //create group
    virtual CPUReservationGroup* createSubgroup(string& name, CPUGroupQoS& qos) 
        throw (CgroupException&)  = 0;    
    //delete sub-group
    virtual void deleteSubgroup(CPUReservationGroup* group) 
        throw (CgroupException&) = 0;
    //get parent group
    virtual CPUReservationGroup* getParentGroup() 
        throw (CgroupException&) = 0;
    //get children groups
    virtual list<CPUReservationGroup*>* getChildrenGroups() 
        throw (CgroupException&) = 0;

protected:
    UInt m_id;    
    string m_name;    
    CPUGroupQoS m_cpuGroupQoS;

    static const int RESERVE_TASK_OP = 0;
    static const int CREATE_SUB_OP = 1;
    static const int DELETE_SUB_OP = 2;
};

#endif	/* _CPURESERVATIONGROUP_H */

