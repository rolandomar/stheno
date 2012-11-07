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
#ifndef EVENTFUTURE_H
#define EVENTFUTURE_H
#include <euryale/Types.h>
#include <euryale/event/Event.h>
#include <euryale/common/ListHelper.h>

template <class RESULT>
class EventFuture : public Event {
public:

    EventFuture(ACE_Time_Value* timeout) : Event(timeout) {
        m_debugEventFuture = RuntimeConfiguration::isClassDebugged("EventFuture");
        m_result = new list<RESULT > ();
        m_expectedSize = 1;
    }

    EventFuture(UInt expectedSize, ACE_Time_Value* timeout) : Event(timeout) {
        m_debugEventFuture = RuntimeConfiguration::isClassDebugged("EventFuture");
        m_result = new list<RESULT > ();
        m_expectedSize = expectedSize;
    }

    virtual ~EventFuture() {
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::~EventFuture()\n")));
        }
        if (m_result != 0) {
            if (m_debugEventFuture) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::~EventFuture() - delete result list\n")));
            }
            ListHelper<RESULT>::deleteList(m_result);
            m_result = 0;
        }
    }

    virtual int setFuture(RESULT result) {
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::setFuture() before lock %@\n"),m_lock));
        }
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *m_lock, -1);
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::setFuture() after lock\n")));
        }
        if (m_result == 0) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: EventFuture::setFuture(%@) - no result\n"), this));
            return -1;
        }
        //m_result = result;
        m_result->push_back(result);
        if (m_result->size() >= m_expectedSize) {
            if (m_debugEventFuture) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::setFuture(%@) - we have all results (%d,%d)\n"), this, m_result->size(), m_expectedSize));
            }
            Event::stateChanged_i(Event::ES_SUCCESS);
            return 0;
        }
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::setFuture(%@) - still missing some result (%d,%d)\n"), this, m_result->size(), m_expectedSize));
        }
        return 0;
    }
    
    virtual list<RESULT>* waitFuture(ACE_Time_Value* timeout) {
        
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture() before lock\n")));
        }
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *m_lock, 0);
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture() after lock\n")));
        }        
        while (true) {
            if (Event::isStateFinal()) {
                if (m_debugEventFuture) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture(%@) already have results\n"), m_lock));
                }
                return detach();
            }
            if (m_debugEventFuture) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture(%@)\n"), m_lock));
            }
            if (Event::wait(timeout) != 0) {
                //if (m_debugEventFuture) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ------------------>EventFuture::waitFuture() timeout<----------------\n")));
                //}
                Event::stateChanged_i(Event::ES_TIMEOUT);
                return detach();
            }
            if (Event::isStateFinal()) {
                return detach();
            } else {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture(): after wait but state not final(%d)\n"),this->getState()));
                //return 0;
            }
        }
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::waitFuture() received result\n")));
        }
        return detach();
    }

    virtual list<RESULT>* detach() {
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::detach() before lock\n")));
        }
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *m_lock, 0);
        if (m_debugEventFuture) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::detach() after lock\n")));
        }
        list<RESULT>* temp = m_result;
        if (m_debugEventFuture) {
            if (temp != 0) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::detach() list size=%d\n"), m_result->size()));
            } else {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventFuture::detach() result is null\n")));
            }
        }
        m_result = 0;
        return temp;
    }

protected:
    bool m_debugEventFuture;
    list<RESULT>* m_result;
    UInt m_expectedSize;

};

#endif // SYNCHRONIZABLEFUTURE_H
