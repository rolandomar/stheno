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
 * File:   CPUPartition.cpp
 * Author: rmartins
 * 
 * Created on February 25, 2010, 4:13 PM
 */

#include "CPUPartition.h"
CPUPartition::CPUPartition()
{}
CPUPartition::CPUPartition(ACE_CString& cgroupName,list<UInt>& cpus):
m_path(cgroupName),m_cpus(cpus)
{    
    //m_cpus = cpus;
}

CPUPartition::CPUPartition(const CPUPartition& orig):
m_path(orig.m_path),m_cpus(orig.m_cpus)
{
}

CPUPartition::~CPUPartition()
{
    m_cpus.clear();
}

