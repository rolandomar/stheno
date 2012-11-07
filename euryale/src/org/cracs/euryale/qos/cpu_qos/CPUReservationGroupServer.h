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
 * File:   CPUReservationGroupServer.h
 * Author: rmartins
 *
 * Created on December 2, 2009, 4:03 PM
 */

#ifndef _CPURESERVATIONGROUPSERVER_H
#define	_CPURESERVATIONGROUPSERVER_H

#include <euryale/qos/cpu_qos/CPUReservationGroup.h>



class CPUReservationGroupServer : public CPUReservationGroup{
public:
    CPUReservationGroupServer();
    virtual ~CPUReservationGroupServer();    
    
    virtual list<ACE_id_t>* getTasks() throw (CgroupException&){
        return 0;
    }
    virtual CPUReservationGroup* getParentGroup() throw (CgroupException&){return 0;}
    virtual list<CPUReservationGroup*>* getChildrenGroups() throw (CgroupException&) {
        return (list<CPUReservationGroup*>*)&m_siblings;
    }

protected:
    list<CPUReservationGroupServer*> m_siblings;
    
    virtual CPUReservation* reserveCPU(ACE_id_t pid, CPUQoS& qos) throw (CgroupException&)
    {
        //CPUCGroup* cpuGroup;
        //reply = cpuGroup->reserve(handle,qos);
        return 0;
    }



};

#endif	/* _CPURESERVATIONGROUPSERVER_H */

