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
 * File:   ParentCellClientHandler.cpp
 * Author: rmartins
 * 
 * Created on August 25, 2010, 6:33 PM
 */

#include "ParentCellClientHandler.h"

ParentCellClientHandler::~ParentCellClientHandler() {
    /*if(m_requests!=0){
        delete m_requests;
    }*/
    close();
}

ParentCellClientHandler::ParentCellClientHandler(UUIDPtr& uuid, CellIDPtr& cellID,
        Cell* cell, bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoPacket, SthenoHeader, //CellPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new CellPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_cellID(cellID) {
    m_debugParentCellClientHandler = RuntimeConfiguration::isClassDebugged("ParentCellClientHandler");
    m_cell = cell;
    //m_requests = new RequestEngine<SthenoPacket*>();
}