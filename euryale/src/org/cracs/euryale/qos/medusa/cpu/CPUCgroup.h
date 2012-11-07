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

#ifndef _CPUCGROUP_H
#define	_CPUCGROUP_H

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

class CPUCgroup : public CgroupSubsystem {
public:
    static const bool INVERT_PARTITIONS = false;
    static const bool EXCLUSIVE_PARTITIONS = true;
    static const bool BALANCE = true;//true;
    static const char*  BALANCE_DOMAINS;// = "1";
    static const char*  BALANCE_RELAX_DOMAINS;// = "3";
    
        
    CPUCgroup(Cgroup* cg);
    virtual ~CPUCgroup();

    void init(bool onlyReset) throw (CgroupException&);
    
    void remove();

    void parseCPUSETCPU(String* data, list<UInt>& cpus);

    void createPartition(String& partitionPath, list<UInt>& cpus,UInt runtime = 0,UInt period =0)
        throw (CgroupException&);
    
    void createRuntimeDomain(String& domain,UInt runtime = 0,UInt period = 0)
        throw (CgroupException&);

    void unbalancePartitions();

    void balancePartitions();

    //
    list<CPUPartition*>* getPartitions();        
    //
    CPUPartition* getCPUReservationGroup(String& groupName);    

    void removeCPUGroup(String& groupName) throw (CgroupException&);

    virtual void getCgroupDirectory(String& path);
protected:
    list<CPUPartition*> m_cpuPartitionsList;

    void init_i(bool onlyReset) throw (CgroupException&);

    void createPreset(bool autoMigrateTasktoBe = false) throw (CgroupException&);
    void createPresetDualCore(bool autoMigrateTasktoBe = false) throw (CgroupException&);
    void createPresetQuadCore(bool autoMigrateTasktoBe = false) throw (CgroupException&);
    //n-core specific

    void convertLisToStringCPU(String& str, list<UInt>& cpus);

};

#endif	/* _CPUCGROUP_H */

