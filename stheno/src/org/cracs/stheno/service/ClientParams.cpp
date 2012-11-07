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
 * File:   ClientParams.cpp
 * Author: rmartins
 * 
 * Created on February 11, 2011, 3:28 PM
 */

#include "ClientParams.h"

ClientParams::ClientParams(int type,int priority):
m_qos(type,priority)
//m_qos(qos),m_priority(priority)
{
}

ClientParams::ClientParams(const ClientParams& orig):
m_qos(orig.m_qos)//,m_priority(orig.m_priority)
{
}

ClientParams::~ClientParams() {
}

QoSEndpoint& ClientParams::getQoS(){
    return m_qos;
}

/*int ClientParams::getPriority(){
    return m_priority;
}*/
