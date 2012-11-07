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
 * File:   MedusaClientHandler.h
 * Author: rmartins
 *
 * Created on December 16, 2009, 3:28 PM
 */

#ifndef _HADESCLIENTHANDLER_H
#define	_HADESCLIENTHANDLER_H

#include <ace/Synch_Traits.h>
//#include <ace/Null_Condition.h>
//#include <ace/Global_Macros.h>
#include <ace/Synch.h>
#include <ace/Condition_T.h>
#include "ace/Recursive_Thread_Mutex.h"

#include <ace/Reactor.h>
#include <ace/INET_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <ace/LSOCK_Connector.h>
//#include <ace/Connector.h>
#include "ace/Synch_Options.h"
#include <ace/Svc_Handler.h>
#include <ace/Reactor_Notification_Strategy.h>
#include <ace/Future.h>
#include <ace/Atomic_Op.h>

#include <euryale/qos/net/packet/MedusaPacket.h>
#include <euryale/qos/exception/CgroupException.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>
#include <euryale/qos/cpu_qos/CPUPartition.h>
#include <euryale/qos/net/packet/ThreadReserveRequestPacket.h>
#include <euryale/qos/net/packet/ThreadReserveReplyPacket.h>
#include <euryale/qos/net/packet/GetPartitionRequestPacket.h>
#include <euryale/qos/net/packet/GetPartitionReplyPacket.h>
#include <euryale/qos/net/packet/CPUGroupReserveRequestPacket.h>
#include <euryale/qos/net/packet/CPUGroupReserveReplyPacket.h>
#include <list>
#include <map>
#include <iostream>
using std::list;
using std::map;
/**
 * @class MedusaClientHandler
 *
 * @brief A hadesd client class.
 *
 * Client class for acessing the QoS primitives of Medusad.
 *
 */

class MedusaClientHandlerCloseListener{
public:
    MedusaClientHandlerCloseListener(){}
    virtual ~MedusaClientHandlerCloseListener(){}
    
    virtual void onMedusaClientHandlerClose() = 0;
};

class MedusaClientHandler :
public ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_MT_SYNCH> {
    typedef ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_MT_SYNCH> super;

public:

    MedusaClientHandler () : 
    m_listener(0),
    notifier_ (0, this, ACE_Event_Handler::WRITE_MASK)
        {}    
    MedusaClientHandler (UUIDPtr& uuid,MedusaClientHandlerCloseListener* listener) : 
    m_uuid(uuid),
    m_listener(listener),
    notifier_ (0, this, ACE_Event_Handler::WRITE_MASK)
        {}    
    
    virtual ~MedusaClientHandler();
    int close(u_long flags = ALL_EVENTS_MASK | DONT_CALL);
    

    virtual int send(MedusaPacket* packet);
    virtual int open(void * = 0);

    /**
     * Retrieves the partition information based on the presets
     * configurared in hadesd. This is directly based on cpuset cgroup.
     * This control group is off limits to the developer, begin totally controlled
     * by hadesd.
     * \exception <CgroupException> {CGroup exception raised by forbidden operation.}
     * @return List of cpu partitions.
     */
    virtual list<CPUPartition*>* getCPUPartitionList() throw(CgroupException&);
    virtual void createRuntimeCPUReserve(
        ACE_CString& scheDomain,CPUGroupQoS& qos) throw(CgroupException&);
    virtual void deleteCPUGroupReserve(ACE_CString& cpuGroup) throw(CgroupException&);
    
    /**
     * Allocates the task number to partitionName partition.
     * @param partitionName Partition Name.
     * @param task Task to be set in partition.
     * * \exception <CgroupException> {Thrown if user isn't allowed to add
     * a new task to the choosen partition..}
     */
    virtual void setTaskInCPUPartition(UInt task,ACE_CString& cpusetGroup,
        ACE_CString& scheDomain,CPUQoS* qos) throw(CgroupException&);

    /**
     * Creates a cpu schedule domain (aka cpu cgroup) reservation group
     * @param cpuGroup Name of the reservation group to be created
     * @return 0 sucess; -1 error
     */
    virtual void createCPUGroupReservation(ACE_CString& cpuGroup) throw(CgroupException&);
    virtual void deleteCPUGroupReservation(ACE_CString& cpuGroup) throw(CgroupException&);    
    /**
     * Retrieves the tasks numbers in a given cgroup.
     * @param cpuGroup CGroup name
     * @return List of tasks currently assigned to the given cgroup.
     */
    virtual list<UInt>* getTasksInCPUGroup(ACE_CString& cpuGroup) throw(CgroupException&);

    // Called when input is available from the client.
    /**
     * Document handle_input(ACE_HANDLE) here...
     * @param fd
     * @return ...
     */
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    // Called when output is possible.
    virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    // Called when a timer expires.
    virtual int handle_timeout(const ACE_Time_Value &current_time,
        const void *act = 0);
    
    virtual int handle_close (ACE_HANDLE = ACE_INVALID_HANDLE,
                            ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

    UInt getNextSequenceID(){
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
        return ++m_sequenceID;
    }

protected:
    virtual int handlePacket(MedusaPacket* packet);



private:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    enum {
        ITERATIONS = 5
    };
    int iterations_;
    ACE_Reactor_Notification_Strategy notifier_;
    ACE_Message_Block* m_begin;
    ACE_Message_Block* m_current;
    MedusaHeader* m_header;
    ACE_Message_Block* m_partialMessage;
    map<UInt,ACE_Future<MedusaPacket*>*> m_pendingRequests;
    ACE_Atomic_Op<ACE_Thread_Mutex, UInt> m_sequenceID;
    MedusaClientHandlerCloseListener* m_listener;
    UUIDPtr m_uuid;
};

#endif	/* _HADESCLIENTHANDLER_H */

