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

#include <list>


#include <ace/OS_NS_string.h>
#include "ace/Log_Msg.h"
#include "ace/streams.h"

#include "CPUCgroup.h"
#include <euryale/common/ListHelper.h>
#include <euryale/qos/medusa/Cgroup.h>

#include <sys/mount.h>

const char* CPUCgroup::BALANCE_DOMAINS = "1"; //1
// relax domains = 3 does not work for kernel >= 3.0
// must be 1
const char* CPUCgroup::BALANCE_RELAX_DOMAINS =  "0";//"3"

CPUCgroup::CPUCgroup(Cgroup* cg) : CgroupSubsystem(cg) {
}

CPUCgroup::~CPUCgroup() {
}

void CPUCgroup::remove() {
    /// mount -t cmountgroup cpuset -ocpuset /cgroups/cpuset
    int ret = -1;
    String path;
    ACE_stat stat;
    getCgroupDirectory(path);
    this->removeTree(path.c_str());

    /*if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        ACE_DEBUG((LM_DEBUG, "CPUCgroup: creating cgroup root dir: %s\n", path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            ACE_DEBUG((LM_DEBUG, "CPUCgroup: Unable to create cpu cgroup dir\n"));
            throw new CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// restart container
    }*/
    String command;
    /*command = "mount -t cgroup -o cpuset none ";

    command += path;
    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
            throw new CgroupException(ACE_OS::strerror(errno));
        }
    } else {
    }*/

    ACE_DEBUG((LM_DEBUG, "CPUCgroup: reseting ...\n"));
    reset(path.c_str());
    ACE_DEBUG((LM_DEBUG, "CPUCgroup: reseted\n"));
    ACE_OS::chdir("/");

    command = "umount ";
    command += path;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)umount=%s\n"), command.c_str()));
    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::remove() umount failed=%s\n"), command.c_str()));
        throw new CgroupException(ACE_OS::strerror(errno));
    } else {
        /// mount ok
        /// reset container
    }
    if (ACE_OS::rmdir(path.c_str()) == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::remove() rmdir failed=%s\n"), path.c_str()));
    }
    ACE_DEBUG((LM_DEBUG, "CPUCgroup: removed!\n"));
}

CPUPartition* CPUCgroup::getCPUReservationGroup(String& groupName) {

    String path;
    //getCgroupDirectory(path);
    path += groupName;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: getCPUReservationGroup() - path=%s groupname=%s\n"), path.c_str(), groupName.c_str()));
    if (ACE_OS::chdir(path.c_str()) == -1) {
        perror("getCPUReservationGroup:");
        return 0;
    }

    String command(path);
    //command += path;
    HierarchicalPath::appendPaths(command, "/cpuset.cpus");
    //command += "/cpuset.cpus";
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: getCPUReservationGroup() - command=%s\n"), command.c_str()));
    String* data = this->readFile(command.c_str());
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: getCPUReservationGroup() - DATA=%s\n"), data->c_str()));

    //ACE_CString cgroupName("/");
    //cgroupName += path;

    list<UInt> cpus;
    this->parseCPUSETCPU(data, cpus);
    CPUPartition* partition = new CPUPartition(path, cpus);
    return partition;
}

void CPUCgroup::removeCPUGroup(String& groupName) throw (CgroupException&) {
    String path;
    getCgroupDirectory(path);
    path += groupName;
    this->removeTree(path.c_str());
}

void CPUCgroup::createPartition(String& partitionPath, list<UInt>& cpus, UInt runtime, UInt period)
throw (CgroupException&) {
    String path;
    int ret;

    getCgroupDirectory(path);
    path += partitionPath;
    String cpusStr;
    this->convertLisToStringCPU(cpusStr, cpus);

    if (ACE_OS::mkdir(path.c_str()) != 0) {
        throw CgroupException(ACE_OS::strerror(errno));
    }
    String command;
    command = "echo 0";
    command += " >  ";
    command += path;
    command += "/cpuset.mems";
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MEM=%s\n"), command.c_str()));

    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        throw CgroupException(ACE_OS::strerror(errno));
    }
    command = "echo ";
    command += cpusStr;
    command += " > ";
    command += path;
    command += "/cpuset.cpus";

    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        throw CgroupException(ACE_OS::strerror(errno));
    }

    //command = "echo 0 > ";
    command = "echo ";
    command += BALANCE_DOMAINS;
    command += " > ";
    command += path;
    command += "/cpuset.sched_load_balance";

    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
      perror("Error=");
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - failed setting balance domain=%s\n"),
		 command.c_str()));
      throw CgroupException(ACE_OS::strerror(errno));
    }

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - ok setting balance domain=%s\n"),
            command.c_str()));


    /*-1	Use the system default value for load balancing
     0	Do not perform immediate load balancing; balance loads only periodically
     1	Immediately balance loads across threads on the same core
     2	Immediately balance loads across cores in the same package
     3	Immediately balance loads across CPUs on the same node or blade
     4	Immediately balance loads across several CPUs on architectures with non-uniform memory access (NUMA)
     5	Immediately balance loads across all CPUs on architectures with NUMA*/
    //command = "echo 0 > ";
    command = "echo ";
    command += BALANCE_RELAX_DOMAINS;
    command += " > ";
    command += path;
    command += "/cpuset.sched_relax_domain_level";

    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - failed relax balance domain=%s\n"),
		 command.c_str()));
        throw CgroupException(ACE_OS::strerror(errno));
    }
      ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - ok setting relax balance domain=%s\n"),
		 command.c_str()));


    //TODO: new code
    if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        path += "/";
        String runtimeCommand(path);
        String periodCommand(path);
        runtimeCommand += "cpu.rt_runtime_us";
        periodCommand += "cpu.rt_period_us";
	//        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - created domain, setting domain runtime=%s: %d\n"),
		   //                path.c_str(), runtime));
        stringstream numRuntime(stringstream::in | stringstream::out);
        numRuntime << runtime;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - creating domain setting domain runtime=%s: %s\n"),
		   runtimeCommand.c_str(), numRuntime.str().c_str()));
        this->writeFile(runtimeCommand.c_str(), numRuntime.str().c_str());
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - OK domain setting domain runtime=%s: %s\n"),
		   runtimeCommand.c_str(), numRuntime.str().c_str()));
        //num.str("\0");
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - creating domain, setting domain period=%s: %d\n"),
        //        path.c_str(), period));
        stringstream numPeriod(stringstream::in | stringstream::out);
        numPeriod << period;
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - creating domain setting domain period=%s: %s\n"),
		   periodCommand.c_str(), numPeriod.str().c_str()));
        this->writeFile(periodCommand.c_str(), numPeriod.str().c_str());
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - OK domain setting domain period=%s: %s\n"),
		   periodCommand.c_str(), numPeriod.str().c_str()));
    }

    m_cpuPartitionsList.push_back(new CPUPartition(partitionPath, cpus));
}

void CPUCgroup::createRuntimeDomain(String& domain, UInt runtime, UInt period) throw (CgroupException&) {
    String path;
    int ret;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CPUCgroup::createRuntimeDomain() domain=%s\n"), domain.c_str()));

    if (Cgroup::PARTITIONING) {
        list<CPUPartition*>* partitions = this->getPartitions();
        list<CPUPartition*>::iterator iter = partitions->begin();
        while (iter != partitions->end()) {
            getCgroupDirectory(path);
            HierarchicalPath::appendPaths(path, (*iter)->getCanonicalName());
            //path = (*iter)->getCanonicalName();

            //path += domain;

            HierarchicalPath hp(path);
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)createRuntimeDomain=%s\n"), path.c_str()));

            //CPUPartition* parentDomain = this->getCPUReservationGroup((*iter)->getCanonicalName());
            CPUPartition* parentDomain = this->getCPUReservationGroup(path);
            if (parentDomain == 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() no parent =%s\n"), domain.c_str()));
                throw CgroupException("Parent domain not found!");
            }

            HierarchicalPath::appendPaths(path, domain);

            String cpusStr;
            this->convertLisToStringCPU(cpusStr, parentDomain->getCpus());
            delete parentDomain;

            if (ACE_OS::mkdir(path.c_str()) != 0) {
                throw CgroupException(ACE_OS::strerror(errno));
            }
            String command;
            command = "echo 0";
            command += " >  ";
            HierarchicalPath::appendPaths(command, path);
            //command += path;        
            //command += "/cpuset.mems";
            HierarchicalPath::appendPaths(command, "/cpuset.mems");
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MEM=%s\n"), command.c_str()));


            ret = ACE_OS::system(command.c_str());
            if (ret != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
                throw CgroupException(ACE_OS::strerror(errno));
            }
            command = "echo ";
            command += cpusStr;
            command += " > ";
            //command += path;
            HierarchicalPath::appendPaths(command, path);
            //command += "/cpuset.cpus";
            HierarchicalPath::appendPaths(command, "/cpuset.cpus");
            ret = ACE_OS::system(command.c_str());
            if (ret != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
                throw CgroupException(ACE_OS::strerror(errno));
            }


            command = "echo ";
            command += BALANCE_DOMAINS; //"1";
            command += " > ";
            //command += path;
            HierarchicalPath::appendPaths(command, path);
            //command += "/cpuset.cpus";
            HierarchicalPath::appendPaths(command, "/cpuset.sched_load_balance");

            ret = ACE_OS::system(command.c_str());
            if (ret != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
                throw CgroupException(ACE_OS::strerror(errno));
            }

            command = "echo ";
            command += BALANCE_RELAX_DOMAINS; //"1";
            command += " > ";
            //command += path;
            HierarchicalPath::appendPaths(command, path);
            //command += "/cpuset.cpus";
            HierarchicalPath::appendPaths(command, "/cpuset.sched_relax_domain_level");

            ret = ACE_OS::system(command.c_str());
            if (ret != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
                throw CgroupException(ACE_OS::strerror(errno));
            }

            //TODO: new code
            if (Cgroup::CPUSET_AND_SCHED_MERGED) {
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
            iter++;
        }
    } else {
        getCgroupDirectory(path);
        HierarchicalPath hp(path);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)createRuntimeDomain=%s\n"), path.c_str()));

        CPUPartition* parentDomain = this->getCPUReservationGroup(path);
        if (parentDomain == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() no parent =%s\n"), domain.c_str()));
            throw CgroupException("Parent domain not found!");
        }

        HierarchicalPath::appendPaths(path, domain);

        String cpusStr;
        this->convertLisToStringCPU(cpusStr, parentDomain->getCpus());
        delete parentDomain;

        if (ACE_OS::mkdir(path.c_str()) != 0) {
            throw CgroupException(ACE_OS::strerror(errno));
        }
        String command;
        command = "echo 0";
        command += " >  ";
        HierarchicalPath::appendPaths(command, path);
        HierarchicalPath::appendPaths(command, "/cpuset.mems");
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)MEM=%s\n"), command.c_str()));


        ret = ACE_OS::system(command.c_str());
        if (ret != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
            throw CgroupException(ACE_OS::strerror(errno));
        }
        command = "echo ";
        command += cpusStr;
        command += " > ";
        //command += path;
        HierarchicalPath::appendPaths(command, path);
        //command += "/cpuset.cpus";
        HierarchicalPath::appendPaths(command, "/cpuset.cpus");
        ret = ACE_OS::system(command.c_str());
        if (ret != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
            throw CgroupException(ACE_OS::strerror(errno));
        }


        command = "echo ";
        command += BALANCE_DOMAINS; //"1";
        command += " > ";
        //command += path;
        HierarchicalPath::appendPaths(command, path);
        //command += "/cpuset.cpus";
        HierarchicalPath::appendPaths(command, "/cpuset.sched_load_balance");

        ret = ACE_OS::system(command.c_str());
        if (ret != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
            throw CgroupException(ACE_OS::strerror(errno));
        }

        command = "echo ";
        command += BALANCE_RELAX_DOMAINS; //"3";
        command += " > ";
        //command += path;
        HierarchicalPath::appendPaths(command, path);
        //command += "/cpuset.cpus";
        HierarchicalPath::appendPaths(command, "/cpuset.sched_relax_domain_level");

        ret = ACE_OS::system(command.c_str());
        if (ret != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createRuntimeDomain() failed=%s\n"), command.c_str()));
            throw CgroupException(ACE_OS::strerror(errno));
        }

        //TODO: new code
        if (Cgroup::CPUSET_AND_SCHED_MERGED) {
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
    }
}

void CPUCgroup::createPresetDualCore(bool autoMigrateTasktoBe) throw (CgroupException&) {
    String bePath("/BE"), /*srtPath("/SRT"), */hrtPath("/HRT");
    list<UInt> beCpus, srtCpus, hrtCpus;
    beCpus.push_back(0);
    //srtCpus.push_back(1);
    //srtCpus.push_back(2);
    hrtCpus.push_back(1);
    if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        //createPartition(bePath, beCpus, 5000, CPUReservation::DEFAULT_PERIOD);
        createPartition(bePath, beCpus, 645000, CPUReservation::DEFAULT_PERIOD);
        //createPartition(srtPath, srtCpus);
        //createPartition(hrtPath, hrtCpus, 900000, CPUReservation::DEFAULT_PERIOD);
        createPartition(hrtPath, hrtCpus, 350000, CPUReservation::DEFAULT_PERIOD);
    } else {
        createPartition(bePath, beCpus);
        //createPartition(srtPath, srtCpus);
        createPartition(hrtPath, hrtCpus);
    }

    if (autoMigrateTasktoBe) {
        ACE_DLList<String> tasks;
        ACE_CString rootTaskFile;
        getCgroupDirectory(rootTaskFile);
        rootTaskFile += "/tasks";
        this->readTasks(rootTaskFile.c_str(), tasks);
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)Task size=%lu\n"), tasks.size()));
        this->migrateTasks(bePath.c_str(), tasks);
    }
}

void CPUCgroup::createPresetQuadCore(bool autoMigrateTasktoBe) throw (CgroupException&) {
    //String bePath("/BE")/*, srtPath("/SRT")*/, hrtPath("/HRT");
    String osPath("/BE")/*, srtPath("/SRT")*/, hrtPath("/HRT");
    list<UInt> osCpus, /*srtCpus,*/ hrtCpus;

    int num = ACE_OS::num_processors();

    if (!INVERT_PARTITIONS) {
        int startCore = 1;
        osCpus.push_back(0);
        if (num > 4) { //Intel HT
            osCpus.push_back(1);
            startCore = 2;
        }
        for (int i = startCore; i < num; i++) {
            hrtCpus.push_back(i);
        }
    } else {
        int endCore = 0;
        if (num > 4) { //Intel HT            
            endCore = 6;
            osCpus.push_back(6);
            osCpus.push_back(7);
        } else {
            endCore = 3;
            osCpus.push_back(3);
        }
        for (int i = 0; i < endCore; i++) {
            hrtCpus.push_back(i);
        }
    }

    if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        createPartition(osPath, osCpus, 645000, CPUReservation::DEFAULT_PERIOD);
        //createPartition(srtPath, srtCpus);
        createPartition(hrtPath, hrtCpus, 350000, CPUReservation::DEFAULT_PERIOD);
    } else {
        createPartition(osPath, osCpus);
        //createPartition(srtPath, srtCpus);        
        createPartition(hrtPath, hrtCpus);
    }

    if (EXCLUSIVE_PARTITIONS) {
        String beFullPath;
        this->getCgroupDirectory(beFullPath);
        HierarchicalPath::appendPaths(beFullPath, osPath);
        beFullPath += "/cpuset.cpu_exclusive";
        writeFile(beFullPath.c_str(), "1");

        String hrtFullPath;
        this->getCgroupDirectory(hrtFullPath);
        HierarchicalPath::appendPaths(hrtFullPath, hrtPath);
        hrtFullPath += "/cpuset.cpu_exclusive";
        writeFile(hrtFullPath.c_str(), "1");
    }

    if (autoMigrateTasktoBe) {
        ACE_DLList<String> tasks;
        ACE_CString rootTaskFile;
        getCgroupDirectory(rootTaskFile);
        rootTaskFile += "/tasks";
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: createPresetQuadCore() - migrating tasks from %s to %s\n"),
                rootTaskFile.c_str(), osPath.c_str()));
        this->readTasks(rootTaskFile.c_str(), tasks);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: createPresetQuadCore() - Task size=%lu\n"), tasks.size()));
        String beFullPath;
        this->getCgroupDirectory(beFullPath);
        HierarchicalPath::appendPaths(beFullPath, osPath);
        this->migrateTasks(beFullPath.c_str(), tasks, true);
        //Only init (1)
        /*String beFullPath;
        this->getCgroupDirectory(beFullPath);
        HierarchicalPath::appendPaths(beFullPath, bePath);
        ACE_DLList<String> tasks;
        tasks.insert_head(new String("1"));
        this->migrateTasks(beFullPath.c_str(), tasks);*/
    }
}

void CPUCgroup::createPreset(bool autoMigrateTasktoBe) throw (CgroupException&) {
    int cores = ACE_OS::num_processors();
    switch (cores) {
        case 1:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createPreset() - single cores aren't supported!")));
            throw CgroupException("Doesn't work with single cores");
        case 2:
        {
            createPresetDualCore(autoMigrateTasktoBe);
            return;
        }
        default:
        {
            createPresetQuadCore(autoMigrateTasktoBe);
            return;
        }
    }
}

void CPUCgroup::init_i(bool onlyReset) throw (CgroupException&) {
    /// mount -t cgroup cpuset -ocpuset /cgroups/cpuset
    int ret = -1;
    String path;
    ACE_stat stat;
    getCgroupDirectory(path);
    if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: creating cgroup root dir: %s\n"), path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            ACE_DEBUG((LM_DEBUG, "CPUCgroup: Unable to create cpu cgroup dir\n"));
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// restart container        
    }
    String command;
    if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        command = "mount -t cgroup cpuset -ocpuset,cpu ";
    } else {
        command = "mount -t cgroup -o cpuset none ";
    }

    command += path;
    ret = ACE_OS::system(command.c_str());
    if (ret != 0) {
        if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CPUCgroup::createPreset() - single cores aren't supported!")));
            throw CgroupException(ACE_OS::strerror(errno));
        }
    } else {
        /// mount ok
        /// reset container
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUCgroup: inited sucessfuly!\n")));
    reset(path.c_str());

    if (Cgroup::CPUSET_AND_SCHED_MERGED) {
        path += "/";
        String runtimeCommand(path);
        String periodCommand(path);
        runtimeCommand += "cpu.rt_runtime_us";
        periodCommand += "cpu.rt_period_us";
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - root setting domain runtime=%s: %d\n"),
                path.c_str(), CPUReservation::ROOT_RUNTIME_US));
        stringstream numRuntime(stringstream::in | stringstream::out);
        numRuntime << CPUReservation::ROOT_RUNTIME_US;
        this->writeFile(runtimeCommand.c_str(), numRuntime.str().c_str());
        //num.str("\0");
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)CPUSET: merged - root setting domain period=%s: %d\n"),
                path.c_str(), CPUReservation::DEFAULT_PERIOD));
        stringstream numPeriod(stringstream::in | stringstream::out);
        numPeriod << CPUReservation::DEFAULT_PERIOD;
        this->writeFile(periodCommand.c_str(), numPeriod.str().c_str());
    }

    //TODO: MUST BALANCE!
    if(BALANCE){
        balancePartitions();
    }else{
        unbalancePartitions();
    }


    if (!onlyReset) {
        //Don't migrate tasks
        if (Cgroup::PARTITIONING) {
            createPreset(false);
        } else {
            if (Cgroup::CPUSET_AND_SCHED_MERGED) {
                String bePath("/BE");
                list<UInt> beCpus;
                //beCpus.push_back(0);
                int num = ACE_OS::num_processors();
                //if(num > 4){
                //    num = 4;
                //}
                for (int i = 0; i < num; i++) {
                    beCpus.push_back(i);
                }
                //createPartition(bePath, beCpus, 500000, CPUReservation::DEFAULT_PERIOD);
                //10%
                createPartition(bePath, beCpus, CPUReservation::BE_RESERVATION, CPUReservation::DEFAULT_PERIOD);
            }

        }
    }
    /*if(BALANCE){
        balancePartitions();
    }else{
        unbalancePartitions();
    }*/
    //unbalancePartitions();
}

void CPUCgroup::getCgroupDirectory(String & path) {
    path = m_cg->getCgroupRootDirectory();
    path += "/cpu";
}

void CPUCgroup::parseCPUSETCPU(String* data, list<UInt>& cpus) {
    int pos = data->find(',', 0);
    if (pos == String::npos) {
        int tempPos = data->find('-', 0);
        if (tempPos == String::npos) {
            stringstream ss(data->c_str());
            UInt cpu = 0;
            ss >> cpu;
            cpus.push_back(cpu);
        } else {
            String lowCpuStr = data->substr(0, tempPos);
            String highCpuStr = data->substr(tempPos + 1, data->length() - tempPos);
            stringstream ssLow(lowCpuStr.c_str());
            stringstream ssHigh(highCpuStr.c_str());
            UInt lowCpu = 0;
            UInt highCpu = 0;
            ssLow >> lowCpu;
            ssHigh >> highCpu;
            //printf("low=%s high=%s\n", lowCpuStr.c_str(), highCpuStr.c_str());
            for (UInt i = lowCpu; i <= highCpu; i++) {
                cpus.push_back(i);
            }
        }

        /*stringstream ss(data->c_str());
        UInt cpu = 0;
        ss >> cpu;
        cpus.push_back(cpu);*/
    } else {
        // Skip delimiters at beginning.
        int pos = 0;
        int delPos = data->find(',', 0);

        while (delPos != String::npos) {
            String temp = data->substr(pos, (delPos - pos) - 1);
            int tempPos = temp.find('-', 0);
            if (tempPos == String::npos) {
                stringstream ss(temp.c_str());
                UInt cpu = 0;
                ss >> cpu;
                cpus.push_back(cpu);
            } else {
                String lowCpuStr = temp.substr(0, tempPos - 1);
                String highCpuStr = temp.substr(tempPos, temp.length() - tempPos);
                stringstream ssLow(lowCpuStr.c_str());
                stringstream ssHigh(highCpuStr.c_str());
                UInt lowCpu = 0;
                UInt highCpu = 0;
                ssLow >> lowCpu;
                ssHigh >> highCpu;
                for (UInt i = lowCpu; i < highCpu; i++) {
                    cpus.push_back(i);
                }
            }
            pos = delPos;
            delPos = data->find(',', delPos);
        }
    }

}

void CPUCgroup::unbalancePartitions() {
    String path;
    getCgroupDirectory(path);
    path += "/cpuset.sched_load_balance";
    const ACE_TCHAR* balance = "0";
    this->writeFile(path.c_str(), balance);

    getCgroupDirectory(path);
    path += "/cpuset.sched_relax_domain_level";
    const ACE_TCHAR* relax = "0";
    this->writeFile(path.c_str(), relax);
}

void CPUCgroup::balancePartitions() {
    String path;
    getCgroupDirectory(path);
    path += "/cpuset.sched_load_balance";
    const ACE_TCHAR* balance = "1";
    this->writeFile(path.c_str(), balance);

    getCgroupDirectory(path);
    path += "/cpuset.sched_relax_domain_level";
    const ACE_TCHAR* relax = BALANCE_RELAX_DOMAINS;
    this->writeFile(path.c_str(), relax);
}

list<CPUPartition*>* CPUCgroup::getPartitions() {
    ACE_DLList<String> dirs;
    String rootPath;
    getCgroupDirectory(rootPath);
    this->readChildrenDirectories(rootPath.c_str(), dirs);
    String* entry;
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)****DIR=%lu\n"), dirs.size()));
    list<CPUPartition*>* partitions = new list<CPUPartition*>();
    for (ACE_DLList_Iterator<String> iter(dirs);
            (entry = iter.next()) != 0;
            iter.advance()) {
        String command(rootPath);
        //ACE_DEBUG((LM_DEBUG, "dir  %s\n",entry->c_str()));
        command += "/";
        command += *entry;
        command += "/cpuset.cpus";
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)command=%s\n"), command.c_str()));
        String* data = this->readFile(command.c_str());
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)DATA=%s\n"), data->c_str()));
        ACE_CString cgroupName("/");
        cgroupName += *entry;

        list<UInt> cpus;
        this->parseCPUSETCPU(data, cpus);
        CPUPartition* partition = new CPUPartition(cgroupName, cpus);
        partitions->push_back(partition);
    }
    return partitions;
}

void CPUCgroup::init(bool onlyReset) throw (CgroupException&) {
    init_i(onlyReset);
}

void CPUCgroup::convertLisToStringCPU(String& str, list<UInt>& cpus) {
    list<UInt>::iterator iter;
    iter = cpus.begin();
    int size = cpus.size();
    stringstream ss;
    for (int i = 0; i < size - 1; i++) {
        ss << *iter << ",";
        iter++;
    }
    ss << *iter;
    str = ss.str().c_str();
}
