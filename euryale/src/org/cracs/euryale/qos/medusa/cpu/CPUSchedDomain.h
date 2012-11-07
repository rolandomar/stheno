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

#ifndef _CPUSCHEDDOMAIN_H
#define	_CPUSCHEDDOMAIN_H
#include <euryale/common/Object.h>
#include <euryale/serialization/Serializable.h>

#include "CGTask.h"
#include <euryale/Types.h>
#include <euryale/common/HierarchicalPath.h>
#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/medusa/cpu/CPUSchedCgroup.h>
//#include <euryale/qos/medusa/cpu/CGTask.h>

#include <list>
#include <sstream>
using namespace std;
class SchedTask;

class CPUSchedDomain : public Object {
public:
    CPUSchedDomain(CPUSchedDomain* parent = 0);
    //CPUSchedDomain(ACE_CString& cgroupName, UInt runtime, UInt period);
    virtual ~CPUSchedDomain();

    /**
     * Get the full name of the partition.
     * @return Full name of partition.
     */
    ACE_CString& getCanonicalName()
    {
        return m_path.getCanonicalPath();
    }

    /**
     * Get the cgroup name.
     * @return CGroup name.
     */
    ACE_CString& getName()
    {
        return m_path.getName();
    }

    virtual const char* objectID() const
    {
        return "29f6140fe51234524a4081b07abe04fd";
    }

    virtual const String& toString()
    {
        ACE_TRACE("Object::toString()");
        if (m_toString == 0) {
            m_toString = new String();
            stringstream ss(stringstream::in | stringstream::out);
            ss << "CPUSchedDomain[Path[";
            ss << m_path.getCanonicalPath();
            ss << "],";
            ss << "Runtime[" << m_runtime << "]";
            ss << "Period[" << m_period << "]";
            m_toString = new String(ss.str().c_str());
        }
        return *m_toString;
    }

    bool valid();


    void createDomain(CPUSchedCgroup* cg, ACE_CString& pathDomain,
        UInt runtime, UInt period) throw (CgroupException*);

    void addSiblindDomain(ACE_CString& pathDomain,
        UInt runtime, UInt period) throw (CgroupException*);
    void removeSiblingDomain(CPUSchedDomain* sibling);

    void addTask(UInt task, UInt runtime, UInt period) throw (CgroupException*);
    void removeTask(UInt task, UInt runtime, UInt period) throw (CgroupException*);

    CPUSchedCgroup* getCG()
    {
        return m_cg;
    }

    UInt getRuntime()
    {
        return m_runtime;
    }

    UInt getPeriod()
    {
        return m_period;
    }

    bool process();

    void readTasks(ACE_DLList<String>& tasks);

    void close();

protected:
    CPUSchedDomain* m_parent;
    CPUSchedCgroup* m_cg;
    UInt m_runtime;
    UInt m_period;
    HierarchicalPath m_path;
    list<CPUSchedDomain*> m_siblings;
    list<SchedTask*> m_tasks;

    void clear();

    virtual bool admitDomain(UInt runtime, UInt period)
    {
        return true;
    }

    virtual bool admitTask(UInt runtime, UInt period)
    {
        return true;
    }

    virtual void balance(){

    }

private:
    CPUSchedDomain(const CPUSchedDomain& orig);
};

#endif	/* _CPUSCHEDDOMAIN_H */

