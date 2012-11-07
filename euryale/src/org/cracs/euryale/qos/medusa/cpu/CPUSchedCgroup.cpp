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
 * File:   CPUCgroup.cpp
 * Author: rmartins
 * 
 * Created on November 28, 2009, 5:06 PM
 */

#include <ace/OS_NS_string.h>
#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include "CPUSchedCgroup.h"
#include <euryale/qos/medusa/Cgroup.h>

#include <sys/mount.h>

CPUSchedCgroup::CPUSchedCgroup(Cgroup* cg) : CgroupSubsystem(cg) {
}

CPUSchedCgroup::~CPUSchedCgroup() {
}

void CPUSchedCgroup::createPreset(bool autoMigrateTasktoBe) throw (CgroupException&) {
    String bePath, rtPath, ftPath;
    ACE_stat stat;
    int ret;
    getCgroupDirectory(bePath);
    bePath += "/BE";
    String beCPUs("0");
    if (ACE_OS::mkdir(bePath.c_str()) != 0) {
        throw CgroupException(ACE_OS::strerror(errno));
    }

    if (autoMigrateTasktoBe) {
        ACE_DLList<String> tasks;
        ACE_CString rootTaskFile;
        getCgroupDirectory(rootTaskFile);
        rootTaskFile += "/tasks";
        this->readTasks(rootTaskFile.c_str(), tasks);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Task size=%lu\n"), tasks.size()));
        this->migrateTasks(bePath.c_str(), tasks);
    }
}

void CPUSchedCgroup::createSchedDomain(const ACE_TCHAR* pathDomain, UInt runtime, UInt period)
throw (CgroupException&) {
    if (runtime > period) {
        throw CgroupException("CPUSchedCgroup: runtime > period!");
    }
    String path;
    ACE_stat stat;
    getCgroupDirectory(path);
    if (pathDomain[0] != '/') {
        path += "/";
    }
    path += pathDomain;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: creating domain: %s\n"), path.c_str()));
    if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: creating dir: %s\n"), path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: Unable to create cpu cgroup dir\n")));
            throw CgroupException(ACE_OS::strerror(errno));
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: created dir: %s\n"), path.c_str()));
    } else {
        /// restart container
    }
    path += "/";
    String runtimeCommand(path);
    String periodCommand(path);
    runtimeCommand += "cpu.rt_runtime_us";
    periodCommand += "cpu.rt_period_us";
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: created domain, setting domain runtime=%s: %d\n"),
            path.c_str(), runtime));
    stringstream numRuntime(stringstream::in | stringstream::out);
    numRuntime << runtime;
    this->writeFile(runtimeCommand.c_str(), numRuntime.str().c_str());
    //num.str("\0");
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: created domain, setting domain period=%s: %d\n"),
            path.c_str(), period));
    stringstream numPeriod(stringstream::in | stringstream::out);
    numPeriod << period;
    this->writeFile(periodCommand.c_str(), numPeriod.str().c_str());

}

void CPUSchedCgroup::init_i() throw (CgroupException&) {
    /// mount -t cgroup cpuset -ocpuset /cgroups/cpuset
    int ret = -1;
    String path;
    ACE_stat stat;
    getCgroupDirectory(path);
    if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: creating cgroup root dir: %s\n"), path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: Unable to create cpu cgroup dir\n")));
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// restart container        
    }
    String command;
    //command = "mount -t cgroup cpu -ocpu ";
    command = "mount -t cgroup -o cpu none ";
    command += path;
    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// mount ok
        /// reset container
    }



    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: mounted sucessfuly (%s)!\n"),command.c_str()));
    reset(path.c_str());
    path += "/";
    String runtimeCommand(path);
    String periodCommand(path);
    runtimeCommand += "cpu.rt_runtime_us";
    periodCommand += "cpu.rt_period_us";
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - created domain, setting domain runtime=%s: %d\n"),
            path.c_str(), CPUReservation::ROOT_RUNTIME_US));
    stringstream numRuntime(stringstream::in | stringstream::out);
    numRuntime << CPUReservation::ROOT_RUNTIME_US;
    this->writeFile(runtimeCommand.c_str(), numRuntime.str().c_str());
    //num.str("\0");
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - created domain, setting domain period=%s: %d\n"),
            path.c_str(), CPUReservation::DEFAULT_PERIOD));
    stringstream numPeriod(stringstream::in | stringstream::out);
    numPeriod << CPUReservation::DEFAULT_PERIOD;
    this->writeFile(periodCommand.c_str(), numPeriod.str().c_str());

    getCgroupDirectory(path);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup: reset sucessfuly!\n")));
    createSchedDomain("BE", CPUReservation::BE_RESERVATION, CPUReservation::DEFAULT_PERIOD);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSchedCgroup:init() sucessfuly!\n")));
    //createSchedDomain("Runtime_abc/kernel",10000,50000);
    //createSchedDomain("Runtime_abc/kernel/overlay",10000,50000);
    //createSchedDomain("Runtime_abc/kernel/overlay/base",9000,50000);
    //createSchedDomain("Runtime_abc/kernel/overlay/services",1000,50000);
    //createPreset();
}

void CPUSchedCgroup::remove() throw (CgroupException&) {
    /// mount -t cgroup cpuset -ocpuset /cgroups/cpuset
    int ret = -1;
    String path;
    ACE_stat stat;
    getCgroupDirectory(path);
    if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: creating cgroup root dir: %s\n"), path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: Unable to create cpu cgroup dir\n")));
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// restart container
    }
    String command;
    //command = "mount -t cgroup cpu -ocpu ";
    command = "mount -t cgroup -o cpu none ";
    command += path;
    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// mount ok
        /// reset container
    }
    //ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: mounted sucessfuly!\n"));
    reset(path.c_str());
    //ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: reset sucessfuly!\n"));

    ACE_OS::chdir("/");

    command = "umount ";
    command += path;
    ret = ACE_OS::system(command.c_str());
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)umount=%s\n"),command.c_str()));
    if (ret != 0) {
        throw CgroupException(ACE_OS::strerror(errno));
    } else {
        /// mount ok
        /// reset container
    }
    ACE_OS::rmdir(path.c_str());
}

void CPUSchedCgroup::getCgroupDirectory(String& path) {
    path = m_cg->getCgroupRootDirectory();
    path += "/cpu_sched";
}
