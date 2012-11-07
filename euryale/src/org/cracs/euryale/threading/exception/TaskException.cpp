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
 * File:   ChannelException.cpp
 * Author: rmartins
 * 
 * Created on December 4, 2009, 1:25 PM
 */

#include "TaskException.h"

//const char* CgroupException::CgroupExceptionMessage = "CgroupExceptionException";
const char* TaskException::CGROUP_FAILURE = "CGroups failure!";

TaskException::TaskException(const char* exception){//:m_messageString(exception) {
    m_messageString = new String(exception);
	// TODO Auto-generated constructor stub

}

TaskException::~TaskException() {
	// TODO Auto-generated destructor stub
    if(m_messageString!=0){
        delete m_messageString;
    }
}
