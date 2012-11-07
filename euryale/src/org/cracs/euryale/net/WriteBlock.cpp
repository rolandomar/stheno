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
 * File:   WriteBlock.cpp
 * Author: rmartins
 * 
 * Created on September 28, 2010, 6:26 PM
 */

#include "WriteBlock.h"

WriteBlock::WriteBlock() : Event(0), m_mb(0), m_status(false) {
}

WriteBlock::WriteBlock(ACE_Time_Value* timeout) :
Event(timeout), m_mb(0), m_status(false) {
}


WriteBlock::WriteBlock(ACE_Message_Block* mb, ACE_Time_Value* timeout) :
Event(timeout), m_mb(mb), m_status(false) {
}

WriteBlock::~WriteBlock() {
    ACE_Message_Block::release(m_mb);
}

ACE_Message_Block* WriteBlock::getMessageBlock() {
    return m_mb;
}

int WriteBlock::wait(ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, Event::lock(), -1);
    while (!m_status) {
        /*if(m_status){
            return 0;
        }*/
        int ret = Event::wait(timeout);
        //printf("WriteBlock::wait ret=%d timeout=%p\n",ret,timeout);
        if (ret == -1) {
            return -1;
        }
    }
    return 0;
}

int WriteBlock::signal(void) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, Event::lock(), -1);
    m_status = true;
    return Event::signal();
}
