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
 * File:   Cgroup.cpp
 * Author: rmartins
 * 
 * Created on November 28, 2009, 5:11 PM
 */

#include "Cgroup.h"
#include <ace/OS_NS_sys_stat.h>
#include <ace/Log_Msg.h>
#include <stdio.h>
#include <string.h>



#include <euryale/qos/medusa/cpu/CPUCgroup.h>
#include <euryale/qos/medusa/cpu/CPUSchedCgroup.h>

Cgroup::Cgroup() {
    m_inited = false;
}

Cgroup::Cgroup(const Cgroup& orig) {
}

Cgroup::~Cgroup() {
}

void Cgroup::close() {

}

void Cgroup::remove() throw (CgroupException&) {
    m_cgCPU = new CPUCgroup(this);
    m_cgCPU->remove();
    if (!Cgroup::CPUSET_AND_SCHED_MERGED) {
        if (!DISABLE_CPU_CGROUP) {
            m_cgCPUSched = new CPUSchedCgroup(this);
            m_cgCPUSched->remove();
        }
    } else {
        m_cgCPUSched = 0;
    }
    delete m_cgCPU;
    if (!Cgroup::CPUSET_AND_SCHED_MERGED) {
        if (!DISABLE_CPU_CGROUP) {
            delete m_cgCPUSched;
        }
    }
}

void Cgroup::init(bool onlyReset) throw (CgroupException&) {
    if (m_inited) {
        throw CgroupException("Ccroups already inited");
    }
    ACE_stat stat;
    if (ACE_OS::lstat(this->getCgroupRootDirectory(), &stat) != 0) {
        //ACE_DEBUG((LM_DEBUG, "creating cgroup root dir\n"));
        if (ACE_OS::mkdir(this->getCgroupRootDirectory()) != 0) {
            //ACE_DEBUG((LM_DEBUG, "Unable to create cgroup root dir\n"));
            throw CgroupException(ACE_OS::strerror(errno));
        }

    }
    try {
        m_cgCPU = new CPUCgroup(this);
        m_cgCPU->init(onlyReset);
        ACE_DEBUG((LM_DEBUG, "CPUSET Cgroup: m_cgCPU sucessfuly:\n"));
        String path;
        m_cgCPU->getCgroupDirectory(path);
        ACE_DEBUG((LM_DEBUG, "CPUSET Cgroup: inited sucessfuly: path=%s\n", path.c_str()));
        //TODO: MUST HAVE THIS
        //m_cgCPU->balancePartitions();
        if (!CPUSET_AND_SCHED_MERGED) {
            if (!DISABLE_CPU_CGROUP) {
                m_cgCPUSched = new CPUSchedCgroup(this);
                m_cgCPUSched->init(onlyReset);
                ACE_DEBUG((LM_DEBUG, "CPUSET Cgroup: m_cgCPUSched sucessfuly:\n"));
            }
            //m_cgCPUSched->getCgroupDirectory(path);
            //ACE_DEBUG((LM_DEBUG, "CPUSCHED Cgroup: inited sucessfuly: path=%s\n", path.c_str()));
        }



    } catch (CgroupException& ex) {
        throw ex;
    }

    m_inited = true;
}

