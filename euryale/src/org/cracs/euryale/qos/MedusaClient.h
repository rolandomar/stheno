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
 * File:   MedusaClient.h
 * Author: rmartins
 *
 * Created on December 2, 2009, 4:10 PM
 */

#ifndef _HADESCLIENT_H
#define	_HADESCLIENT_H

#include <ace/OS_NS_Thread.h>

#include <euryale/qos/ReservationGroup.h>
#include <euryale/qos/cpu_qos/CPUReservation.h>
#include <euryale/qos/cpu_qos/CPUReservationGroup.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/io_qos/IOReservation.h>
#include <euryale/qos/io_qos/IOReservationGroup.h>
#include <euryale/qos/io_qos/IOQoS.h>
#include <euryale/qos/mem_qos/MemReservation.h>
#include <euryale/qos/mem_qos/MemReservationGroup.h>
#include <euryale/qos/mem_qos/MemQoS.h>
#include <euryale/qos/net_qos/NetReservation.h>
#include <euryale/qos/net_qos/NetReservationGroup.h>
#include <euryale/qos/net_qos/NetQoS.h>
#include <euryale/qos/net/MedusaClientHandler.h>

#include <ace/Task.h>

#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>


#include <string>
using std::string;

class MedusaClientCloseListener{
public:
    MedusaClientCloseListener(){}
    virtual ~MedusaClientCloseListener(){}
    
    virtual void onMedusaClientClose() = 0;
};

/*
 * QoS hades client.
 * Medusa default implementation is a frontend to the cgroups infrastructure.
 *
 */
class MedusaClient : public ACE_Task_Base, MedusaClientHandlerCloseListener {
public:

    MedusaClient();
    virtual ~MedusaClient();

    int svc(void);
    void open(UUIDPtr& uuid) throw (CgroupException&);
    void close();
    
    void setCloseListener(MedusaClientCloseListener* listener);

    virtual void onMedusaClientHandlerClose();

    CPUReservation* createCPUReservation(
            String& cpusetDomain, String& scheduleDomain, CPUQoS* qos) throw (CgroupException&);

    list<CPUPartition*>* getCPUPartitionList() throw (CgroupException&);

    void createGroupCPUReserve(String& runtimeUUID, CPUGroupQoS& qos) throw (CgroupException&) {
        m_clientHandler->createRuntimeCPUReserve(runtimeUUID, qos);
    }

    void setTaskInCPUPartition(UInt task, String& cpusetGroup,
            String& scheDomain, CPUQoS* qos) throw (CgroupException&) {
        m_clientHandler->setTaskInCPUPartition(task, cpusetGroup, scheDomain, qos);
    }

    /*
    //get cpu group by name
    CPUReservationGroup* getCPUReservationGroup(string& groupName) {
    }
    //create cpu reservation group

    CPUReservationGroup* createCPUReservationGroup(string& groupName, CPUGroupQoS& qos) {
        return 0;
    }

    CPUReservation* reserveCPU(ACE_id_t pid, CPUReservationGroup& group, CPUQoS& qos) throw (CgroupException&) {
        return 0;
    }
    
    //get tasks in cpu group
    virtual list<ACE_id_t>* getTasks(CPUReservationGroup& group) throw (CgroupException&) {
        return 0;
    }

    //get sub-groups in cpu group

    list<CPUReservationGroup*>* getChildrenGroups(CPUReservationGroup& group) throw (CgroupException&) {
        return 0;
    }

    void removeCPUGroup(string& groupName) throw (CgroupException&) {
    }

    void removeCPUGroupTree(string& groupName) throw (CgroupException&) {
    }
    //?
    //CPUReservationGroup* createRootCPUReservationGroup(CPUQoS& qos){return 0;}

    NetReservationGroup* getRootNetReservationGroup() {
        return 0;
    }

    NetReservationGroup* getNetReservationGroup(string& name) {
        return 0;
    }

    NetReservationGroup* createRootNetReservationGroup(NetQoS& qos) {
        return 0;
    }

    void removeNetGroup(NetReservationGroup*) {
    }

    IOReservationGroup* getRootIOReservationGroup() {
        return 0;
    }

    IOReservationGroup* getIOReservationGroup(string& name) {
        return 0;
    }

    IOReservationGroup* createRootIOReservationGroup(IOQoS& qos) {
        return 0;
    }

    void removeIOGroup(IOReservationGroup*) {
    }

    MemReservationGroup* getRootMemReservationGroup() {
        return 0;
    }

    MemReservationGroup* getMemReservationGroup(string& name) {
        return 0;
    }

    MemReservationGroup* createRootMemReservationGroup(CPUQoS& qos) {
        return 0;
    }

    void removeMemGroup(MemReservationGroup*) {
    }
     */

    pid_t getTid(){
        return m_tid;
    }
    
    ACE_thread_t getThreadHandler(){
        return m_th;
    }

protected:
    //UUIDPtr m_uuid;
    MedusaClientCloseListener* m_listener;
    MedusaClientHandler* m_clientHandler;
    ACE_Reactor* m_reactor;
    pid_t m_tid;
    ACE_thread_t m_th;


private:
    ACE_Reactor* allocateReactorImpl(void);



};

typedef ACE_Singleton<MedusaClient, ACE_Thread_Mutex> GlobalMedusaClient;


#endif	/* _HADESCLIENT_H */

