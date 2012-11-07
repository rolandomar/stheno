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
 * Event_.cpp
 *
 *  Created on: May 8, 2008
 *      Author: rmartins
 */
//#if !defined (ACE_TEMPLATES_REQUIRE_SOURCE)

//#include <euryale/runtime/tp/lf/LeaderFollower.h>
//#endif
#include <euryale/event/Event.h>

#include "euryale/runtime/RuntimeConfiguration.h"

/*namespace pt {

namespace efacec {

namespace rto {

namespace event {*/

Event::Event(ACE_Time_Value* timeout, EventListener* listener, ACE_Timer_Heap* timerQueue) :
m_state(Event::ES_IDLE), m_listener(listener), m_timerQueue(timerQueue) {
    m_debugEvent = RuntimeConfiguration::isClassDebugged("Event");
    m_timeout = timeout;
    if (m_timeout != 0 && m_timerQueue != 0) {
        //one time shot
        m_timerQueue->schedule(this, (const void*) 0, *timeout, ACE_Time_Value::zero);
    }
}

Event::~Event() {
    if (m_debugEvent) {
        printf("Event::~Event\n");
    }
    //if (m_debugEvent) {
    //printf("Event::timeout()\n");
    //}
}

void Event::stateChanged(int newState, bool signalOnChange) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    stateChanged_i(newState, signalOnChange);
}

void Event::stateChanged_i(int newState, bool signalOnChange) {
    //if (this->isStateFinal()) {
        //printf("Event:stateChanged_i - is final state\n");
        int oldState = m_state;
        //fireEventChange(oldState, m_state);
        this->stateChanged_ii(newState);
        if (signalOnChange) {
            signal(); //signal if appliable            
        }
        //ace_mon.release();                
        //after release lock,  notify listener
        fireEventChange(oldState, m_state);
        return;
    //}else{
        //printf("Event:stateChanged_i - not final state\n");
    //}
}

int Event::keepWaiting(void) {
    //return (this->successful()==0) && (!this->errorDetected()==0);
    return (!this->successful() || (!this->errorDetected()));
}

void Event::resetState(int newState) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    ///this->m_state = newState;
    this->stateChanged_i(newState,true);
    //signal();
}

void Event::setListener(EventListener * listener) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    //printf("Event::setListener()\n");
    if (m_listener != 0) {
        printf("Event::setListener() - listneer already is set\n");
    }
    m_listener = listener;
    if(this->isStateFinal()){        
        fireEventChange(m_state,m_state);
    }
    //}
}

void Event::removeListener() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    //printf("Event::removeListener()\n");
    m_listener = 0;
}

void Event::cancel() {
    //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    if (m_debugEvent) {
        printf("Event::Cancel()\n");
    }
    resetState(ES_CANCEL);
}

void Event::timeout() {
    //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, *m_lock);
    resetState(ES_TIMEOUT);
}

/// Return 1 if the condition was satisfied successfully, 0 if it
/// has not

bool Event::successful(void) const {
    return (m_state == ES_SUCCESS);
}

bool Event::isCancel(void) const {
    return (m_state == ES_CANCEL);
}

/// Return 1 if an error was detected while waiting for the
/// event

bool Event::errorDetected(void) const {
    return (m_state == ES_ERROR);
}

int Event::getState() {
    return m_state;
}

bool Event::isStateFinal(void) {
    switch (getState()) {
        case ES_ERROR:
        case ES_SUCCESS:
        case ES_TIMEOUT:
        case ES_CANCEL:
            return true;
        default:
            return false;
    }
}

int Event::handle_timeout(const ACE_Time_Value &current_time,
        const void *act) {
    if (m_debugEvent) {
        printf("Event::timeout()\n");
    }
    timeout();
    return 0;
}

int Event::handle_close(ACE_HANDLE handle,
        ACE_Reactor_Mask close_mask) {
    if (m_debugEvent) {
        printf("Event::close()\n");
    }
    cancel();
    return 0;
}

void Event::fireEventChange(int oldState, int newState) {
    if (m_listener != 0) {
        //printf("Event::fireEventChange() before\n");
        m_listener->onEvent(this, oldState, newState);
        //printf("Event::fireEventChange() after\n");
    }
}

void Event::stateChanged_ii(int newState) {
    //int oldState = m_state;
    m_state = newState;
    //fireEventChange(oldState, m_state);
}


/*}

}

}

}
 */
