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
 * File:   CPUPartition.cpp
 * Author: rmartins
 * 
 * Created on February 25, 2010, 4:13 PM
 */

#include "CPUSchedDomain.h"
#include <euryale/qos/medusa/cpu/CGTask.h>
#include <euryale/qos/medusa/CgroupSubsystem.h>
#include <ace/OS_NS_string.h>
#include "ace/Log_Msg.h"
#include "ace/streams.h"

CPUSchedDomain::CPUSchedDomain(CPUSchedDomain* parent):
m_parent(parent)
{
}

/*CPUSchedDomain::CPUSchedDomain(ACE_CString& cgroupName,UInt runtime, UInt period):
m_path(cgroupName),m_runtime(runtime),m_period(period)
{        
}*/

CPUSchedDomain::CPUSchedDomain(const CPUSchedDomain& orig) :
m_path(orig.m_path), m_runtime(orig.m_runtime), m_period(orig.m_period)
{
}

CPUSchedDomain::~CPUSchedDomain()
{
    clear();
}

void CPUSchedDomain::clear()
{
    list<CPUSchedDomain*>::iterator iter;
    iter = m_siblings.begin();
    while (iter != m_siblings.end()) {
        delete *iter;
        iter = m_siblings.erase(iter);
    }
    list<SchedTask*>::iterator iter2;
    iter2 = m_tasks.begin();
    while (iter2 != m_tasks.end()) {
        delete *iter2;
        iter2 = m_tasks.erase(iter2);
    }
}

void CPUSchedDomain::createDomain(CPUSchedCgroup* cg, ACE_CString& pathDomain,
    UInt runtime, UInt period) throw (CgroupException*)
{
    //m_path = cgroupName;
    m_cg = cg;
    m_runtime = runtime;
    m_period = period;
    if (runtime > period) {
        throw new CgroupException("CPUSchedCgroup: runtime > period!");
    }
    String path;
    ACE_stat stat;
    cg->getCgroupDirectory(path);
    path += "/";
    path += pathDomain;
    m_path = path;
    ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: creating domain: %s\n", path.c_str()));
    if (ACE_OS::lstat(path.c_str(), &stat) != 0) {
        ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: creating dir: %s\n", path.c_str()));
        if (ACE_OS::mkdir(path.c_str()) != 0) {
            ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: Unable to create cpu cgroup dir\n"));
            throw new CgroupException(ACE_OS::strerror(errno));
        }
        ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: created dir: %s\n", path.c_str()));
    } else {
        /// restart container
    }
    path += "/";
    String runtimeCommand(path);
    String periodCommand(path);
    runtimeCommand += "cpu.rt_runtime_us";
    periodCommand += "cpu.rt_period_us";
    ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: creating domain, setting domain runtime=%s: %d\n", path.c_str(), runtime));
    stringstream numRuntime(stringstream::in | stringstream::out);
    numRuntime << runtime;
    cg->writeFile(runtimeCommand.c_str(), numRuntime.str().c_str());
    //num.str("\0");
    ACE_DEBUG((LM_DEBUG, "CPUSchedCgroup: creating domain, setting domain period=%s: %d\n", path.c_str(), period));
    stringstream numPeriod(stringstream::in | stringstream::out);
    numPeriod << period;
    cg->writeFile(periodCommand.c_str(), numPeriod.str().c_str());
}

void CPUSchedDomain::addSiblindDomain(ACE_CString& pathDomain,
    UInt runtime, UInt period) throw (CgroupException*)
{
    if (admitDomain(runtime, period)) {
        CPUSchedDomain* sibling = new CPUSchedDomain();
        sibling->createDomain(m_cg, pathDomain, runtime, period);
        m_siblings.push_back(sibling);
        return;
    } else {
        throw new CgroupException("scheduleDomain exception!");
    }
}

void CPUSchedDomain::removeSiblingDomain(CPUSchedDomain* sibling)
{
    list<CPUSchedDomain*>::iterator iter;
    iter = m_siblings.begin();
    while (iter != m_siblings.end()) {
        if (*iter == sibling) {
            m_siblings.remove(sibling);
            sibling->close();
            return;
        }
        iter++;
    }
}

void CPUSchedDomain::addTask(UInt taskNo, UInt runtime, UInt period) throw (CgroupException*)
{
    if (admitTask(runtime, period)) {
        String filePath = m_path.getCanonicalPath();
        filePath += "/tasks";
        SchedTask* task = new SchedTask(taskNo,*m_cg, filePath, runtime, period, this);
        task->assign();
        m_tasks.push_back(task);
        return;
    } else {
        throw new CgroupException("scheduleDomain exception!");
    }
}

void CPUSchedDomain::readTasks(ACE_DLList<String>& tasks)
{
    String filePath(m_path.getCanonicalPath());
    filePath += "/tasks";
    CgroupSubsystem::readTasks(filePath.c_str(), tasks);
}

bool CPUSchedDomain::valid(){
    return CgroupSubsystem::dirExists(m_path.getCanonicalPath().c_str());
}

void CPUSchedDomain::close()
{
    list<CPUSchedDomain*>::iterator iterSiblings;
    iterSiblings = m_siblings.begin();
    while (iterSiblings != m_siblings.end()) {
        (*iterSiblings)->close();
        iterSiblings++;
    }

    list<SchedTask*>::iterator iter;
    iter = m_tasks.begin();
    while (iter != m_tasks.end()) {
        iter++;
    }
    ACE_DLList<String> tasks;
    /// gather info from 'tasks' files
    m_cg->reset_i(m_path.getCanonicalPath().c_str(), tasks);
    /// migrate tasks to root container
    String rootPath;
    m_cg->getCgroupDirectory(rootPath);
    m_cg->migrateTasks(rootPath.c_str(), tasks);
    /// remove sub-tree
    m_cg->removeTree(m_path.getCanonicalPath().c_str());
    delete this;
}

bool CPUSchedDomain::process()
{
    bool scheduleFlag = false;
    /*
     Process domain
     */
    if(!valid()){
        close();
        return false;
    }

    list<CPUSchedDomain*>::iterator iter;
    iter = m_siblings.begin();
    while (iter != m_siblings.end()) {
        if(!(*iter)->process()){
            iter = m_siblings.erase(iter);//already erased
            scheduleFlag = true;
            continue;
        }
        iter++;
    }

    String taskPath;
    taskPath = m_path.getCanonicalPath();
    taskPath += "/tasks";
    list<UInt> tasks;
    CgroupSubsystem::readTasks(taskPath.c_str(),tasks);

    //validate tasks
    list<SchedTask*>::iterator iterTasks;
    
    iterTasks = m_tasks.begin();
    while (iterTasks != m_tasks.end()) {
        SchedTask* t = *iterTasks;
        if (!t->valid(tasks)) {
            iterTasks = m_tasks.erase(iterTasks);
            scheduleFlag = true;
            delete t;
            continue;
        }
        iterTasks++;
    }
    if(scheduleFlag){
        balance();
    }
    return true;
}

void CPUSchedDomain::removeTask(UInt task, UInt runtime, UInt period) throw (CgroupException*)
{

}
