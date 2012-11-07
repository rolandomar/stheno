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
 * File:   CPUPartition.h
 * Author: rmartins
 *
 * Created on February 25, 2010, 4:13 PM
 */

#ifndef _CGTASK_H
#define	_CGTASK_H
#include <euryale/common/Object.h>
#include <euryale/serialization/Serializable.h>
#include <euryale/Types.h>
#include <euryale/common/HierarchicalPath.h>
#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/medusa/cpu/CPUSchedDomain.h>


#include <list>
#include <sstream>
using namespace std;

class CgroupSubsystem;
class CGTask : public Object {
public:
    CGTask(UInt task,CgroupSubsystem& cg,String& path,UInt runtime, UInt period) :
    m_runtime(runtime), m_period(period),m_path(path),m_cg(cg)
    {

    }
    virtual ~CGTask();

    bool valid();
    virtual const char* objectID() const
    {
        return "59f6140fe51234524a4081b07abe04fd";
    }

    void assign() throw(CgroupException*);

    virtual const String& toString()
    {
        ACE_TRACE("Object::toString()");
        if (m_toString == 0) {
            m_toString = new String();
            stringstream ss(stringstream::in | stringstream::out);
            ss << "Task[";
            ss << "Runtime[" << m_runtime << "]";
            ss << "Period[" << m_period << "]";
            m_toString = new String(ss.str().c_str());
        }
        return *m_toString;
    }

protected:
    CgroupSubsystem& m_cg;
    String m_path;
    UInt m_task;
    UInt m_runtime;
    UInt m_period;

    CGTask(const CGTask& orig);
};

class CPUSchedDomain;
class SchedTask : public CGTask {
public:

    SchedTask(UInt task,CgroupSubsystem& cg,String path,UInt runtime, UInt period,CPUSchedDomain* domain) :
    CGTask(task,cg,path,runtime,period),m_domain(domain)
    {
    }

    virtual ~SchedTask()
    {
    }

    bool valid(list<UInt>& tasks){
        list<UInt>::iterator iter;
        iter = tasks.begin();
        while(iter != tasks.end()){
            if((*iter) == m_task){
                return true;
            }
            iter++;
        }
        return false;
    }

protected:
    CPUSchedDomain* m_domain;
};

#endif	/* _CPUSCHEDDOMAIN_H */

