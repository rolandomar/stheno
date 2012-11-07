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
 * File:   InfoUpdate.cpp
 * Author: rmartins
 * 
 * Created on September 29, 2010, 12:09 PM
 */

#include "InfoUpdate.h"

InfoUpdate::InfoUpdate():m_type(0),m_operation(0) {}


InfoUpdate::InfoUpdate(UInt type,UInt operation):m_type(type),m_operation(operation) {
}

InfoUpdate::InfoUpdate(const InfoUpdate& orig):
m_type(orig.m_type),m_operation(orig.m_operation){
}

InfoUpdate::InfoUpdate(InputStream& inputStream):
m_type(0),m_operation(0)
{
    this->deserialize(inputStream);
}

InfoUpdate::~InfoUpdate() {
}

UInt InfoUpdate::getType(){
    return m_type;
}

UInt InfoUpdate::getOperation(){
    return m_operation;
}
    
void InfoUpdate::serialize(OutputStream& outputStream) throw (SerializationException&){
    outputStream.write_ulong(m_type);
    outputStream.write_ulong(m_operation);
}

void InfoUpdate::deserialize(InputStream& inputStream) throw (SerializationException&){
    inputStream.read_ulong(m_type);
    inputStream.read_ulong(m_operation);
    //printf("InfoUpdate::deserialize: %d %d",m_type,m_operation);
}