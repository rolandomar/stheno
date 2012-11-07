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
 * File:   WriteDatagramBlock.cpp
 * Author: rmartins
 * 
 * Created on September 28, 2010, 5:59 PM
 */

#include "WriteDatagramBlock.h"

WriteDatagramBlock::WriteDatagramBlock(ACE_Time_Value* timeout) :
Event(timeout), m_mb(0), m_addr(0), m_status(false) {
}

WriteDatagramBlock::WriteDatagramBlock(ACE_Message_Block* mb, ACE_INET_Addr* addr, ACE_Time_Value* timeout) :
Event(timeout), m_mb(mb), m_addr(addr), m_status(false) {
}

WriteDatagramBlock::~WriteDatagramBlock() {
    ACE_Message_Block::release(m_mb);
}

ACE_Message_Block* WriteDatagramBlock::getMessageBlock() {
    return m_mb;
}

ACE_INET_Addr* WriteDatagramBlock::getAddr() {
    return m_addr;
}

int WriteDatagramBlock::wait(ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, Event::lock(), -1);
    if (m_status) {
        return 0;
    }
    return Event::wait(timeout);
}

int WriteDatagramBlock::signal(void) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, Event::lock(), -1);
    m_status = true;
    return Event::signal();
}
