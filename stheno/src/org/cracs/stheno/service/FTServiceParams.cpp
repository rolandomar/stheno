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
 * File:   FTServiceParams.cpp
 * Author: rmartins
 * 
 * Created on June 26, 2010, 11:27 AM
 */

#include "FTServiceParams.h"


FTServiceParams::FTServiceParams(UInt replicaCount,Byte policy):
m_replicaCount(replicaCount),m_policy(policy)
{
    
}

FTServiceParams::FTServiceParams(InputStream& is){
    this->deserialize(is);
}

FTServiceParams::~FTServiceParams() {
}

void FTServiceParams::serialize(OutputStream& outputStream) throw (SerializationException&){
    outputStream.write_ulong(m_replicaCount);
    outputStream.write_char((char&)m_policy);
}

void FTServiceParams::deserialize(InputStream& inputStream) throw (SerializationException&){
    inputStream.read_ulong(m_replicaCount);
    inputStream.read_char((char&)m_policy);
}