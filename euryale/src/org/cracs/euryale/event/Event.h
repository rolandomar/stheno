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
 * Event_.h
 *
 *  Created on: May 8, 2008
 *      Author: rmartins
 */

#ifndef EVENT_H_
#define EVENT_H_

//#include "ace/Recursive_Thread_Mutex.h"
//#include "ace/Condition_Recursive_Thread_Mutex.h"
#include <ace/Time_Value.h>
#include <euryale/common/Synchronizable.h>
#include <ace/Event_Handler.h>
#include <ace/Timer_Heap.h>
#include <euryale/runtime/RuntimeConfiguration.h>

class Event;

class EventListener {
public:
    virtual void onEvent(Event* event, int oldState, int newState) = 0;
};

class Event : public Synchronizable, public ACE_Event_Handler {
public:

    enum {
        /// The event is created, and is in initial state
        ES_IDLE = 0,
        /// The event is active
        SS_ACTIVE,
        /// The event has completed successfully
        ES_SUCCESS,
        /// A failure has been detected while the event was active
        ES_ERROR,
        /// The event has timed out
        ES_TIMEOUT,
        ES_CANCEL
    };

    Event(ACE_Time_Value* timeout = 0, EventListener* listener = 0, ACE_Timer_Heap* timerQueue = 0);
    virtual ~Event();

    ACE_Time_Value* getTimeout() {
        return m_timeout;
    }

    void setListener(EventListener* listener);
    void removeListener();

    void stateChanged(int newState, bool signalOnChange = true);
    void stateChanged_i(int newState, bool signalOnChange = true);

    virtual void cancel();
    virtual void timeout();

    virtual bool successful(void) const;
    virtual bool isCancel(void) const;
    virtual bool errorDetected(void) const;

    /// Check if we should keep waiting.
    int keepWaiting(void);
    //@}

    /// Reset the state, irrespective of the previous states
    void resetState(int newState);

    virtual int getState();

    virtual bool isStateFinal(void);

    virtual int handle_timeout(const ACE_Time_Value &current_time,
            const void *act = 0);

    virtual int handle_close(ACE_HANDLE handle,
            ACE_Reactor_Mask close_mask);

protected:
    bool m_debugEvent;
    ACE_Timer_Heap* m_timerQueue;
    EventListener* m_listener;
    ACE_Time_Value *m_timeout;
    int m_state;
    /// Validate the state change

    virtual void fireEventChange(int oldState, int newState);
    virtual void stateChanged_ii(int newState);
   

};

#endif /* EVENT__H_ */
