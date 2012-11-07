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

#include "CGTask.h"

#include <ace/OS_NS_string.h>
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include <euryale/qos/medusa/CgroupSubsystem.h>

CGTask::~CGTask()
{
}

bool CGTask::valid()
{
    return CgroupSubsystem::fileExists(m_path.c_str());
}

void CGTask::assign() throw (CgroupException*)
{
    m_cg.setTask(m_path.c_str(), m_task);
}
