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
 * File:   CPUQoS.cpp
 * Author: rmartins
 * 
 * Created on November 3, 2009, 4:49 PM
 */

#include "CPUQoS.h"

CPUQoS::CPUQoS():
m_policy(0)
{
}

CPUQoS::CPUQoS(int type):
m_policy(type)
{
}

CPUQoS::CPUQoS(const CPUQoS& orig)
{
    m_policy = orig.m_policy;
}

CPUQoS::~CPUQoS()
{
}

