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
 * File:   CPUReservePacket.cpp
 * Author: rmartins
 * 
 * Created on December 17, 2009, 12:49 PM
 */

#include "ThreadReserveRequestPacket.h"

ThreadReserveRequestPacket::ThreadReserveRequestPacket(UInt requestID,UInt task,ACE_CString& cpuGroup,
        ACE_CString& cpuSchedGroup,CPUQoS* qos):
MedusaPacket(THREAD_RESERVE_REQUEST_PACKET_ID,requestID),m_task(task),
    m_cgroup(cpuGroup),m_scheGroup(cpuSchedGroup),m_qos(qos)
{
}


ThreadReserveRequestPacket::~ThreadReserveRequestPacket()
{
    if(m_qos!=0){
        delete m_qos;
    }
}

