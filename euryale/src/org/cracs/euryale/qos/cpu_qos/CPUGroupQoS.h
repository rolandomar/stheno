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
 * File:   CPUGroupQoS.h
 * Author: rmartins
 *
 * Created on December 18, 2009, 3:44 PM
 */

#ifndef _CPUGROUPQOS_H
#define	_CPUGROUPQOS_H

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>

class CPUGroupQoS : public Serializable {
public:
    CPUGroupQoS();
    CPUGroupQoS(
            String cpusetDomain,
            String schedDomain,
            UInt runtime,
            UInt period,
            UInt bestEffortWeigth);
    CPUGroupQoS(const CPUGroupQoS& orig);
    virtual ~CPUGroupQoS();

    virtual void serialize(OutputStream& outputStream)
    throw (SerializationException&);

    virtual void deserialize(InputStream& inputStream)
    throw (SerializationException&);

    UInt getRuntime();

    UInt getPeriod();

    UInt getBestEffortWeigth();

    String& getCpusetDomain();

    String& getCPUScheduleDomain();

    CPUGroupQoS & operator =(CPUGroupQoS const& qos);

protected:
    String m_cpuSet;
    String m_cpuScheduleDomain;
    UInt m_runtime;
    UInt m_period;
    UInt m_bestEffortWeigth;

};

#endif	/* _CPUGROUPQOS_H */

