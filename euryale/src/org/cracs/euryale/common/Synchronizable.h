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
 * File:   Synchronizable.h
 * Author: rmartins
 *
 * Created on April 22, 2010, 5:44 PM
 */

#ifndef _SYNCHRONIZABLE_H
#define	_SYNCHRONIZABLE_H

#include <ace/Synch.h>
#include <ace/Condition_T.h>

class Synchronizable {
public:
    Synchronizable();
    Synchronizable(ACE_SYNCH_RECURSIVE_MUTEX& lock);
    Synchronizable(ACE_SYNCH_RECURSIVE_MUTEX* lock, bool release = true);
    virtual ~Synchronizable();

    virtual int wait(ACE_Time_Value* timeout = 0);

    virtual int signal(void);

    ACE_SYNCH_RECURSIVE_MUTEX& lock() {
        return *m_lock;
    }

protected:
    ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX> *m_cond;
    ACE_SYNCH_RECURSIVE_MUTEX* m_lock;
    bool m_release;
    bool m_debugSynchronizable;

};

#endif	/* _SYNCHRONIZABLE_H */

