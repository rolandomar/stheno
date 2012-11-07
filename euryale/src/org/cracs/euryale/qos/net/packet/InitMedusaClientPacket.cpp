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
 * File:   InitMedusaClientPacket.cpp
 * Author: rmartins
 * 
 * Created on January 11, 2011, 4:06 PM
 */

#include "InitMedusaClientPacket.h"

InitMedusaClientPacket::InitMedusaClientPacket() :
MedusaPacket(CLIENT_INIT_ID, 0){
}


InitMedusaClientPacket::InitMedusaClientPacket(UUIDPtr& clientID,UInt requestID) :
MedusaPacket(CLIENT_INIT_ID, requestID),m_clientID(clientID) {
}

InitMedusaClientPacket::~InitMedusaClientPacket() {
}

InitMedusaClientPacket::InitMedusaClientPacket(MedusaHeader* packetHeader) :
MedusaPacket(packetHeader) {

}

void
InitMedusaClientPacket::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt bodySize = getBodySerializationSize();
    ((MedusaHeader*) m_packetHeader)->m_messageSize = bodySize;
    serializeHeader(outputStream);
    serializeBody(outputStream);
}

void
InitMedusaClientPacket::serializeBody(OutputStream& outputStream) throw (SerializationException&) {
    m_clientID->serialize(outputStream);
}

void
InitMedusaClientPacket::deserializeBody(InputStream& inputStream) throw (SerializationException&) {
    UUID* clientID = new UUID(inputStream);
    m_clientID.reset(clientID);
}

UUIDPtr& InitMedusaClientPacket::getClientUUID(){
    return m_clientID;
}
