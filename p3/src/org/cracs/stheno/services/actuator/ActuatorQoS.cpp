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
 * File:   ActuatorQoS.cpp
 * Author: rmartins
 * 
 * Created on October 26, 2010, 1:20 PM
 */

#include "ActuatorQoS.h"

ActuatorQoS::ActuatorQoS(UInt frameRate, UInt frameSize): 
m_frameRate(frameRate),
m_frameSize(frameSize) {
}

ActuatorQoS::ActuatorQoS(InputStream& inputStream) throw (SerializationException&):
m_frameRate(0),
m_frameSize(0)
{
    this->deserialize(inputStream);
}

ActuatorQoS::~ActuatorQoS() {
}

void ActuatorQoS::serialize(OutputStream& outputStream) throw (SerializationException&) {
    outputStream.write_ulong(m_frameSize);
    outputStream.write_ulong(m_frameRate);
}

void ActuatorQoS::deserialize(InputStream& inputStream) throw (SerializationException&) {
    inputStream.read_ulong(m_frameSize);
    inputStream.read_ulong(m_frameRate);
}