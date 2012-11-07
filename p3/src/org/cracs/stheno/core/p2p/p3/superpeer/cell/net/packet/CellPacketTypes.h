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
 * File:   CellPacketTypes.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 5:33 PM
 */

#ifndef CELLPACKETTYPES_H
#define	CELLPACKETTYPES_H

#include <euryale/Types.h>
static const UInt JOIN_CELL_PACKET = 0x000011;
static const UInt JOIN_CELL_REPLY_PACKET = 0x000012;
static const UInt JOIN_MESH_PACKET = 0x000013;
static const UInt JOIN_MESH_REPLY_PACKET = 0x000014;
static const UInt LEAVE_MESH_PACKET = 0x000015;
static const UInt LEAVE_MESH_REPLY_PACKET = 0x000016;
static const UInt REBIND_MESH_PACKET = 0x000017;
static const UInt REBIND_MESH_REPLY_PACKET = 0x000018;
static const UInt UPDATEINFO_MESH_PACKET = 0x000019;
static const UInt UPDATEINFO_MESH_REPLY_PACKET = 0x000020;




#endif	/* CELLPACKETTYPES_H */

