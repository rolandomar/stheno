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
 * File:   CPUGroupReserveReplyPacket.cpp
 * Author: rmartins
 * 
 * Created on December 18, 2009, 3:31 PM
 */

#include "CPUGroupReserveReplyPacket.h"

CPUGroupReserveReplyPacket::CPUGroupReserveReplyPacket(UInt requestID,UInt replyID,UInt reply):
MedusaPacket(CPUGROUP_RESERVE_REPLY_PACKET_ID,requestID),m_replyID(replyID),m_reply(reply)
{
}


CPUGroupReserveReplyPacket::~CPUGroupReserveReplyPacket()
{
}
