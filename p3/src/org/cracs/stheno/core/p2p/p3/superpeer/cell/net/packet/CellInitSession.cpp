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
 * File:   CellInitSession.cpp
 * Author: rmartins
 * 
 * Created on December 10, 2010, 11:43 AM
 */

#include "CellInitSession.h"

CellInitSession::CellInitSession() {
}

CellInitSession::CellInitSession(InputStream& inputStream) {
    this->deserialize(inputStream);
}

CellInitSession::CellInitSession(
        UUIDPtr& srcPID,
        UUIDPtr& srcFID,
        UUIDPtr& dstPID,
        UUIDPtr& dstFID,
        ULong requestID
        ) :
SthenoPacket(srcPID, srcFID, dstPID, dstFID, CELL_INIT_SESSION_PACKET_OP, requestID) {
}

CellInitSession::~CellInitSession() {
}

void
CellInitSession::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}



void
CellInitSession::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
}

void
CellInitSession::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
}


