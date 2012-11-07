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
 * File:   CellClientHandler.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 9, 2010, 1:00 PM
 */

#include "CellClientHandler.h"

CellClientHandler::~CellClientHandler() {    
    close();
}

CellClientHandler::CellClientHandler() {
    m_debugClientHandler = RuntimeConfiguration::isClassDebugged("CellClientHandler");
    throw -1;
}

CellClientHandler::CellClientHandler(UUIDPtr& uuid, Cell* cell, bool asyncRead, bool asyncWrite,                                     
                                     ExecutionModelPtr* ec,
                                     NetReservation* reserve,
                                     NetQoS* qos,
                                     CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new CellPacketFactory(),asyncRead, asyncWrite, ec, reserve, qos, closeListener), m_uuid(uuid) {
    m_debugClientHandler = RuntimeConfiguration::isClassDebugged("CellClientHandler");
    m_cell = cell;
    //m_requests = new RequestEngine<SthenoPacket*>();
}