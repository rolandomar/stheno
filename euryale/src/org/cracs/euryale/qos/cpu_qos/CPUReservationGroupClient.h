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
 * File:   CPUReservationGroupClient.h
 * Author: rmartins
 *
 * Created on December 2, 2009, 4:03 PM
 */

#ifndef _CPURESERVATIONGROUPCLIENT_H
#define	_CPURESERVATIONGROUPCLIENT_H

#include <euryale/qos/cpu_qos/CPUReservationGroup.h>
class MedusaClient;
class CPUReservationGroupClient: public CPUReservationGroup {
public:
    CPUReservationGroupClient(MedusaClient* client);
    /*virtual ~CPUReservationGroupClient();

    virtual CPUReservation* reserveCPU(ACE_id_t pid,CPUQoS& qos) throw (CgroupException&);
    virtual list<ACE_id_t>* getTasks() throw (CgroupException&);
    //create a sub-domain, we cannot use CPUQoS for this, we 
    virtual CPUReservationGroup* createSubgroup(string& name,CPUGroupQoS& qos) throw (CgroupException&);
    virtual void deleteSubgroup(CPUReservationGroup* group) throw (CgroupException&);
    virtual void pruneSubgroupTree(CPUReservationGroup* group) throw (CgroupException&);
    virtual CPUReservationGroup* getParentGroup() throw (CgroupException&);
    virtual list<CPUReservationGroup*>* getChildrenGroups() throw (CgroupException&);*/
    
private:    
    MedusaClient* m_client;
};

#endif	/* _CPURESERVATIONGROUPCLIENT_H */

