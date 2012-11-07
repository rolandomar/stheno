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
 * File:   P3DiscoveryPackets.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 27, 2010, 2:43 PM
 */

#ifndef P3DISCOVERYPACKETS_H
#define	P3DISCOVERYPACKETS_H
#include <euryale/Types.h>

static const UInt REQUEST_CELL_PACKET = 0x000001;
static const UInt REQUEST_CELL_REPLY_PACKET = 0x000002;
static const UInt NOTIFY_PACKET = 0x000003;
static const UInt FIND_COORDINATOR_PACKET = 0x000004;
static const UInt FIND_COORDINATOR_REPLY_PACKET = 0x000005;
static const UInt REQUEST_COORDINATOR_PACKET = 0x000006;
static const UInt REQUEST_COORDINATOR_REPLY_PACKET = 0x000007;


#endif	/* P3DISCOVERYPACKETS_H */

