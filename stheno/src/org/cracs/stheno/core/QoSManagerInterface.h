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
 * File:   QoSManagerInterface.h
 * Author: rmartins
 *
 * Created on January 12, 2011, 5:36 PM
 */

#ifndef QOSMANAGERINTERFACE_H
#define	QOSMANAGERINTERFACE_H

#include <list>
using std::list;

#include <euryale/Types.h>
#include <euryale/qos/exception/CgroupException.h>

//Forward references
class SthenoCore;
class CPUQoS;
class CPUPartition;
class CPUReservation;
class CPUGroupQoS;

class QoSManagerInterface {
public:
    friend class SthenoCore;
    virtual ~QoSManagerInterface();

    CPUReservation* createCPUReservation(
            String& cpusetDomain,
            //String& scheduleDomain,
            CPUQoS* qos) throw (CgroupException&);
    CPUReservation* createCPUReservation(
            const char* cpusetDomain,
            //String& scheduleDomain,
            CPUQoS* qos) throw (CgroupException&);

    list<CPUPartition*>* getCPUPartitionList() throw (CgroupException&);

    void createGroupCPUReserve(
            String& runtimeUUID,
            CPUGroupQoS& qos) throw (CgroupException&);

    void setTaskInCPUPartition(
            UInt task,
            String& cpusetGroup,
            //String& scheDomain,
            CPUQoS* qos) throw (CgroupException&);

    QoSManagerInterface* createSubDomainQoSManager(
            String& path,
            ULong runtime,
            ULong period) throw (CgroupException&);
    
    QoSManagerInterface* createSubDomainQoSManager(
            const char* path,
            ULong runtime,
            ULong period) throw (CgroupException&);

private:
    SthenoCore* m_core;
    String m_rootPath;

    QoSManagerInterface(SthenoCore* core, String& rootPath, ULong runtime, ULong period,bool create = true) throw (CgroupException&);
    QoSManagerInterface(SthenoCore* core, const char* rootPath, ULong runtime, ULong period,bool create = true) throw (CgroupException&);
    String& getRootPath();
};

#endif	/* QOSMANAGERINTERFACE_H */

