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
 * File:   QoSManager.h
 * Author: rmartins
 *
 * Created on January 6, 2011, 4:39 PM
 */

#ifndef QOSMANAGER_H
#define	QOSMANAGER_H

#include <euryale/qos/MedusaClient.h>
//#include <stheno/exception/RuntimeException.h>
class SthenoCore;

class QoSManager : public MedusaClientCloseListener {
public:
    QoSManager(SthenoCore* runtime);
    virtual ~QoSManager();

    void open() throw (CgroupException&);
    void close();
    void onMedusaClientClose();

    CPUReservation* createCPUReservation(                
            String& cpusetDomain,
            String& scheduleDomain,
            CPUQoS* qos) throw (CgroupException&);

    list<CPUPartition*>* getCPUPartitionList() throw (CgroupException&);

    void createGroupCPUReserve(
            String& runtimeUUID,
            CPUGroupQoS& qos) throw (CgroupException&);

    void setTaskInCPUPartition(
            UInt task,
            String& cpusetGroup,
            String& scheDomain,
            CPUQoS* qos) throw (CgroupException&);
protected:
    SthenoCore* m_runtime;
    MedusaClient* m_medusaClient;
    bool m_closed;
    bool m_started;

};

#endif	/* QOSMANAGER_H */

