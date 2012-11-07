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

#ifndef _CPUPARTITION_H
#define	_CPUPARTITION_H
#include <euryale/common/Object.h>
#include <euryale/serialization/Serializable.h>
#include <euryale/Types.h>
#include <euryale/common/HierarchicalPath.h>
#include <list>
#include <sstream>
using namespace std;

class CPUPartition : public Object, public Serializable {
public:
    CPUPartition();
    CPUPartition(ACE_CString& cgroupName, list<UInt>& cpus);
    virtual ~CPUPartition();

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
    
    list<UInt>& getCpus(){
        return m_cpus;
    }

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&)
    {
        outputStream.write_string(m_path.getCanonicalPath());
        UInt listSize = m_cpus.size();
        outputStream.write_ulong(listSize);
        list<UInt>::iterator iter;
        iter = m_cpus.begin();
        while (iter != m_cpus.end()) {
            outputStream.write_ulong(*iter);
            iter++;
        }
    }

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&)
    {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CPUPartition::deserialize\n")));
        ACE_CString path;
        inputStream.read_string(path);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CPUPartition::deserialize path=%s\n"), path.c_str()));
        m_path.setPath(path);
        UInt listSize = 0;
        inputStream.read_ulong(listSize);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CPUPartition::deserialize listSize=%d\n"), listSize));
        m_cpus.clear();
        UInt temp;
        for (int i = 0; i < listSize; i++) {
            inputStream.read_ulong(temp);
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CPUPartition::deserialize cpu=%d\n"), temp));
            m_cpus.push_back(temp);
        }

    }

    virtual const char* objectID() const
    {
        return "1246140fe51234524a4081b07d7e04dd";
    }

    virtual const String& toString()
    {
        ACE_TRACE("Object::toString()");
        if (m_toString == 0) {
            //m_toString = new String("CPU Partition[Path[");
            m_toString = new String();
            stringstream ss(stringstream::in | stringstream::out);
            //m_toString << m_path.getCanonicalPath();
            //ACE_CString k;
            ss << "CPU Partition(Path(";
            ss << m_path.getCanonicalPath();
            ss << "),";
            ss << "CPUs(";
            list<UInt>::iterator iter = m_cpus.begin();
            UInt size = m_cpus.size();
            for(int i=0; i < size-1; i++){
            //while(iter != m_cpus.end()){
                ss << *iter << ",";
                iter++;
            }            
            ss << *iter;
            ss << ")";
            m_toString = new String(ss.str().c_str());
        }
        return *m_toString;
    }

protected:
    list<UInt> m_cpus;
    HierarchicalPath m_path;

private:
    CPUPartition(const CPUPartition& orig);
};

#endif	/* _CPUPARTITION_H */

