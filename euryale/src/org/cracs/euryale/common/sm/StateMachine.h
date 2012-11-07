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
 * File:   StateMachine.h
 * Author: rmartins
 *
 * Created on August 3, 2010, 4:09 PM
 */

#ifndef STATEMACHINE_H
#define	STATEMACHINE_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <euryale/Types.h>
#include <euryale/common/sm/exception/StateMachineException.h>

class StateMachine {
public:
    enum {
        NOT_INIT = 0x1,
        STARTING,
        RUNNING,
        SUSPENDING,
        SUSPENDED,
        RESUMING,
        CLOSING,
        CLOSED
    };
    
    StateMachine();
    StateMachine(const StateMachine& orig);
    virtual ~StateMachine();
    
    void open() throw(StateMachineException&);    
    void close() throw(StateMachineException&);    
    void resume() throw(StateMachineException&);    
    void suspend() throw(StateMachineException&);

    bool isValid();

    bool isSuspended();

    bool isClose();

    Byte getStatus();
    
    bool isStarting();
protected:
    Byte m_status;
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    
    virtual void open_i() throw (StateMachineException&){}
    virtual void close_i() throw(StateMachineException&){}
    virtual void resume_i() throw(StateMachineException&){}
    virtual void suspend_i() throw(StateMachineException&){}
    
    void toStarting() throw(StateMachineException&);

    void toStart() throw(StateMachineException&);

    void toSuspending() throw(StateMachineException&);

    void toSuspended() throw(StateMachineException&);
    
    void toResuming() throw(StateMachineException&);

    void toClosing() throw(StateMachineException&);

    void toClose() throw(StateMachineException&);

    void changeState(int newState) throw(StateMachineException&);

};

#endif	/* STATEMACHINE_H */

