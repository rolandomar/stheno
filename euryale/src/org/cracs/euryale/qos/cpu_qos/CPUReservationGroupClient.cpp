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
 * File:   CPUReservationGroupClient.cpp
 * Author: rmartins
 * 
 * Created on December 2, 2009, 4:03 PM
 */

#include "CPUReservationGroupClient.h"
#include <euryale/qos/MedusaClient.h>

CPUReservationGroupClient::CPUReservationGroupClient(MedusaClient* client) :
m_client(client) {
}
/*
CPUReservationGroupClient::~CPUReservationGroupClient() {
    try{
        m_client->removeCPUGroup(m_name);
    }catch(CgroupException& ex){
        //catch and log
        printf("Error: CPUReservationGroupClient::~CPUReservationGroupClient()\n");
    }
}

CPUReservation* CPUReservationGroupClient::reserveCPU(ACE_id_t pid, CPUQoS& qos) throw (CgroupException&) {
    return m_client->reserveCPU(pid, *this, qos);
}

CPUReservationGroup* CPUReservationGroupClient::createSubgroup(string& name, CPUGroupQoS& qos) throw (CgroupException&) {
    string childName = this->m_name + "/" + name;
    return m_client->createCPUReservationGroup(childName, qos);
}


CPUReservationGroup* CPUReservationGroupClient::getParentGroup() throw (CgroupException&){
    string parentGroupName;
    return m_client->getCPUReservationGroup(parentGroupName);
}

void CPUReservationGroupClient::deleteSubgroup(CPUReservationGroup* group) throw (CgroupException&){
    //throw CgroupException("Not implemented!");
    m_client->removeCPUGroup(group->getName());
}

void CPUReservationGroupClient::pruneSubgroupTree(CPUReservationGroup* group) throw (CgroupException&){
    m_client->removeCPUGroupTree(group->getName());
}

list<CPUReservationGroup*>* CPUReservationGroupClient::getChildrenGroups() throw (CgroupException&){
    return m_client->getChildrenGroups(*this);
}


list<ACE_id_t>* CPUReservationGroupClient::getTasks() throw (CgroupException&){
    return m_client->getTasks(*this);
}
*/