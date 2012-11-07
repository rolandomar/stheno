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
 * File:   CPUCgroup.h
 * Author: rmartins
 *
 * Created on November 28, 2009, 5:06 PM
 */

#ifndef _CPUSCHEDCGROUP_H
#define	_CPUSCHEDCGROUP_H

#include <string>
#include <list>
using std::string;
using std::list;

#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/medusa/CgroupSubsystem.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/qos/cpu_qos/CPUReservationGroup.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUPartition.h>

class CPUSchedDomain;
class CPUSchedCgroup : public CgroupSubsystem {
public:
    CPUSchedCgroup(Cgroup* cg);
    virtual ~CPUSchedCgroup();

    void remove() throw (CgroupException&);

    void init(bool onlyReset) throw (CgroupException&)
    {
        init_i();
    }

    void close(){}

    void createSchedDomain(const ACE_TCHAR* path,UInt runtime,UInt period) throw (CgroupException&);
   
    void getCgroupDirectory(String& path);

    
protected:
    void init_i() throw (CgroupException&);

    void createPreset(bool autoMigrateTasktoBe = false) throw (CgroupException&);

    CPUSchedDomain* m_rootDomain;

};

#endif	/* _CPUSCHEDCGROUP_H */

