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
 * File:   MedusadCore.h
 * Author: rmartins
 *
 * Created on November 28, 2009, 4:34 PM
 */

#ifndef _HADESDCORE_H
#define	_HADESDCORE_H

#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/medusa/net/MedusadAcceptor.h>
#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/qos/cpu_qos/CPUPartition.h>

#include "Cgroup.h"
#include "cpu/CPUCgroup.h"
#include "cpu/CPUSchedCgroup.h"
#include <euryale/common/ListHelper.h>
class Cgroup;
class MedusaServiceHandler;

class MedusadCore {
public:
    MedusadCore();
    MedusadCore(const MedusadCore& orig);

    /// service task run point
    void start();
    void init(bool onlyReset = false) throw (CgroupException&);
    void remove() throw (CgroupException&);

    virtual ~MedusadCore();

    void close();



    void processPacket(MedusaPacket* packet, MedusaServiceHandler* handler);
    void onCloseHandle(MedusaServiceHandler* handler);
    void onOpenHandle(MedusaServiceHandler* handler);

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    MedusadAcceptor* m_acceptor;
    Cgroup* m_cg;
    bool m_stop;

    void run();

    /**
     * Document createSchedDomainReserve(String&,UInt,UInt) here...
     * @param schedDomain
     * @param runtime
     * @param period
     */
    void createRuntimeReservation(String& schedDomain, UInt runtime, UInt period) throw (CgroupException&) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)InFO: createRuntimeReservation()\n")));
        //TODO
        m_cg->getCPUSetGroup()->createRuntimeDomain(schedDomain, runtime, period);
        if (!Cgroup::DISABLE_CPU_CGROUP) {
            if (!Cgroup::CPUSET_AND_SCHED_MERGED) {
                m_cg->getCPUSchedGroup()->createSchedDomain(schedDomain.c_str(), runtime, period);
            }
        }
    }

    //cpuset=BE/uuid/ sched=uuid/

    void setTaskInCPUReserve(UInt taskID, String& cpusetGroup, String& schedDomain) {
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: setTaskInCPUReserve %d %s %s\n"), taskID, cpusetGroup.c_str(), schedDomain.c_str()));
                
        if(!Cgroup::PARTITIONING){
            cpusetGroup = "";
        }
        String mergedDomain(cpusetGroup);

        
        //if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        HierarchicalPath::appendPaths(mergedDomain, schedDomain);
        //}
        
            //ACE_Time_Value start = ACE_OS::gettimeofday();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Seeting cpuset(%s)\n"), mergedDomain.c_str()));
        //TODO
        m_cg->getCPUSetGroup()->setTask(mergedDomain.c_str(), taskID);
        //ACE_Time_Value middle = ACE_OS::gettimeofday();
        //m_cg->getCPUSetGroup()->setTask(cpusetGroup.c_str(), taskID);
        if (!Cgroup::CPUSET_AND_SCHED_MERGED) {
            if (!Cgroup::DISABLE_CPU_CGROUP) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Seeting scheddomain\n")));
                m_cg->getCPUSchedGroup()->setTask(schedDomain.c_str(), taskID);
            }
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Seeting End\n")));
        }
        //ACE_Time_Value end = ACE_OS::gettimeofday();
        //ACE_UINT64 t1;
        //ACE_UINT64 t2;
        //ACE_UINT64 t3;
        //(middle-start).to_usec(t1);
        //(end-middle).to_usec(t2);
        //(end-start).to_usec(t3);
        //ACE_DEBUG((LM_DEBUG, 
        //ACE_TEXT("(%t|%T)INFO: setTaskInCPUReserve() times: cpuset=%d cpu=%d total=%d\n"),t1,t2,t3));

    }

    //ACE_Message_Block* processPacket(ACE_Message_Block* command);

    list<CPUPartition*>* getCPUPartitions() {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: getCPUPartitions()\n")));
        return m_cg->getCPUSetGroup()->getPartitions();
    }

};

#endif	/* _HADESDCORE_H */

