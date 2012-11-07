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
 * File:   RTParams.cpp
 * Author: rmartins
 * 
 * Created on August 12, 2010, 4:47 PM
 */

#include "RTParams.h"

RTParams::RTParams(ACE_Time_Value* deadline) :
m_deadline(deadline) {
}

RTParams::~RTParams() {
    if (m_deadline != 0) {
        delete m_deadline;
    }
}

ACE_Time_Value* RTParams::getDeadline() {
    return m_deadline;
}