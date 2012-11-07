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
 * File:   Synchronizable.cpp
 * Author: rmartins
 *
 * Created on April 22, 2010, 5:44 PM
 */

#include "Synchronizable.h"
#include <euryale/runtime/RuntimeConfiguration.h>

Synchronizable::Synchronizable() :
m_release(true), m_lock(0), m_cond(0), m_debugSynchronizable(false) {
    m_debugSynchronizable = RuntimeConfiguration::isClassDebugged("Synchronizable");
    m_release = true;
    m_lock = new ACE_SYNCH_RECURSIVE_MUTEX;
    m_cond = new ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX > (*m_lock);
}

Synchronizable::~Synchronizable(){
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Synchronizable::~Synchronizable() lock(%@)\n"),m_lock));
    if (m_release) {
        if (m_lock != 0) {
            delete m_lock;
        }
    }
    if (m_cond != 0) {
        delete m_cond;
    }
}

Synchronizable::Synchronizable(ACE_SYNCH_RECURSIVE_MUTEX& lock) :
m_release(true), m_lock(0), m_cond(0), m_debugSynchronizable(false) {
    m_debugSynchronizable = RuntimeConfiguration::isClassDebugged("Synchronizable");
    m_lock = &lock;
    m_release = false;
    m_cond = new ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX > (lock);
}

Synchronizable::Synchronizable(ACE_SYNCH_RECURSIVE_MUTEX* lock, bool release) :
m_release(release), m_lock(0), m_cond(0) {
    m_debugSynchronizable = RuntimeConfiguration::isClassDebugged("Synchronizable");
    m_release = release;
    m_lock = lock;
    m_cond = new ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX > (*lock);
}

int Synchronizable::wait(ACE_Time_Value* timeout) {
    if (m_cond != 0) {
        if (timeout != 0) {
            //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock,-1);
            if (m_debugSynchronizable) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::wait(%@) with timeout\n"), m_lock));
            }
            int ret = m_cond->wait(timeout);
            if (m_debugSynchronizable) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::wait(%@) after signal with timeout\n"), m_lock));
            }
            return ret;
        } else {
            //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock,-1);
            if (m_debugSynchronizable) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::wait(%@) without timeout\n"), m_lock));
            }
            int ret = m_cond->wait();
            if (m_debugSynchronizable) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::wait(%@) after signal without timeout\n"), m_lock));
            }
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Synchronizable::wait(%@) - signal received.\n"),m_lock));
            return ret;
        }
    }
    return -1;
}

int Synchronizable::signal(void) {
    if (m_cond != 0) {
        //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock,-1);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: Synchronizable::signal(%@)\n"),m_lock));
        if (m_debugSynchronizable) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::signal(%@) before signal\n"), m_lock));
        }
        int ret = m_cond->signal();
        if (m_debugSynchronizable) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::signal(%@) after signal\n"), m_lock));
        }
        return ret;
    }
    if (m_debugSynchronizable) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Synchronizable::signal(%@) without cond?\n"), m_lock));
    }
    return -1;
}
