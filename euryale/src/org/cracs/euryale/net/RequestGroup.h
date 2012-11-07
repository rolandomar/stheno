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
 * File:   RequestGroup.h
 * Author: rmartins
 *
 * Created on September 23, 2010, 11:07 AM
 */

#ifndef REQUESTGROUP_H
#define	REQUESTGROUP_H

#include <euryale/net/ChannelRequest.h>

template <class CHANNEL, class REQUEST>
class RequestGroup : public ChannelRequest<CHANNEL, REQUEST>/*, public EventListener */ {
public:

    RequestGroup(ACE_Time_Value* timeout) : ChannelRequest<CHANNEL, REQUEST>(0, timeout) {
        m_debugRequestGroup = RuntimeConfiguration::isClassDebugged("RequestGroup");
        ChannelRequest<CHANNEL, REQUEST>::m_expectedSize = 0;
        m_finishedRequests = 0;
        m_opened = false;
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::RequestGroup(\n")));
        }
    }

    virtual ~RequestGroup() {
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::~RequestGroup() - %@\n"), ChannelRequest<CHANNEL, REQUEST>::m_result));
        }
        if (ChannelRequest<CHANNEL, REQUEST>::m_result != 0) {
            typename list<REQUEST>::iterator iter = ChannelRequest<CHANNEL, REQUEST>::m_result->begin();
            while (iter != ChannelRequest<CHANNEL, REQUEST>::m_result->end()) {
                (*(*iter))->removeListener();
                (*(*iter))->cancel();
                iter++;
            }
        }
    }

    virtual int addRequest(REQUEST result) {
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::setFuture() - before lock\n")));
        }
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock), -1);
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::setFuture() - after lock\n")));
        }

        if (ChannelRequest<CHANNEL, REQUEST>::m_result == 0) {
            if (m_debugRequestGroup) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: RequestGroup::setFuture(%@) - no result\n"), Synchronizable::m_lock));
            }
            return -1;
        }
        //m_result = result;
        ChannelRequest<CHANNEL, REQUEST>::m_result->push_back(result);
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::setFuture() after push\n")));
        }
        ChannelRequest<CHANNEL, REQUEST>::m_expectedSize++;

        /*f (isAllEventFinal()) {
            if (m_debugRequestGroup) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::setFuture() all events finished\n")));
            }
            Event::stateChanged(Event::ES_SUCCESS);
            return 0;
        }*/

        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::setFuture() events pendings\n")));
        }
        return 0;
    }

    virtual list<REQUEST>* waitFuture(ACE_Time_Value* timeout) {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock), 0);
        if (isAllEventFinal()) {
            Event::stateChanged_i(Event::ES_SUCCESS);
             return detach();
        }
        //mon.release();
        //return ChannelRequest<CHANNEL, REQUEST>::waitFuture(timeout);            
        while (true) {
            if (ChannelRequest<CHANNEL, REQUEST>::isStateFinal()) {
                if (m_debugRequestGroup) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::waitFuture(%@) already have results\n"), ChannelRequest<CHANNEL, REQUEST>::m_lock));
                }
                return detach();
            }
            if (m_debugRequestGroup) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::waitFuture(%@)\n"), ChannelRequest<CHANNEL, REQUEST>::m_lock));
            }
            if (ChannelRequest<CHANNEL, REQUEST>::wait(timeout) != 0) {
                //if (m_debugEventFuture) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: ------------------>RequestGroup::waitFuture() timeout<----------------\n")));
                //}
                ChannelRequest<CHANNEL, REQUEST>::stateChanged_i(Event::ES_TIMEOUT);
                return detach();
            }
            if (ChannelRequest<CHANNEL, REQUEST>::isStateFinal()) {
                return detach();
            } else {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::waitFuture(): after wait but state not final(%d)\n"),this->getState()));
                //return 0;
            }
        }
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::waitFuture() received result\n")));
        }
        return detach();

    }

    virtual int open() {
        ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock), -1);
        if (m_opened) {
            return -1;
        }
        m_opened = true;
        typename list<REQUEST>::iterator iter = ChannelRequest<CHANNEL, REQUEST>::m_result->begin();
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::open()\n")));
        }
        while (iter != ChannelRequest<CHANNEL, REQUEST>::m_result->end()) {
            if ((*(*iter))->isStateFinal()) {
                if (m_debugRequestGroup) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::open() event is already finished...\n")));
                }
                m_finishedRequests++;
            } else {
                if (m_debugRequestGroup) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::open() setting listener...\n")));
                }
                (*(*iter))->setListener(this);
            }
            iter++;
        }
        return 0;
    }

    virtual void onEvent(Event* event, int oldState, int newState) {
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::onEvent() before lock - this=%@ lock=%@ event=%@\n"), this, ChannelRequest<CHANNEL, REQUEST>::m_lock,event));
        }
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock));
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::onEvent() after lock - this=%@ event=%@\n"), this, event));
        }

        if (event != 0 && ChannelRequest<CHANNEL, REQUEST>::m_wb!=0 && event == static_cast<Event*> (ChannelRequest<CHANNEL, REQUEST>::m_wb->get())) {
            if (event->isStateFinal()) {
                (*ChannelRequest<CHANNEL, REQUEST>::m_wb)->removeListener();
                delete ChannelRequest<CHANNEL, REQUEST>::m_wb;
                ChannelRequest<CHANNEL, REQUEST>::m_wb = 0;
                return;
            } else {
                if (event->getState() == Event::ES_CANCEL || event->getState() == Event::ES_ERROR || event->getState() == Event::ES_TIMEOUT) {
                    cancel(true);
                    return;
                }
            }
        }


        if (event->isStateFinal()) {
            if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::onEvent() this event is final! this=%@ event=%@\n"), this, event));
        }
            m_finishedRequests++;
        }

        if (isAllEventFinal()) {
            if (m_debugRequestGroup) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::onEvent() this=%@ %@ SUCESS!! %d %d\n"), this, event, m_finishedRequests, ChannelRequest<CHANNEL, REQUEST>::m_expectedSize));
            }
            Event::stateChanged_i(Event::ES_SUCCESS);
        } else {
            if (m_debugRequestGroup) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::onEvent() this=%@ %@ still not SUCESS!! %d %d\n"), this, event, m_finishedRequests, ChannelRequest<CHANNEL, REQUEST>::m_expectedSize));
            }
        }
    }

    virtual void cancel(bool clearMap = true) {
        if (m_debugRequestGroup) {
            printf("RequestGroup::cancel(%p)\n", this);
        }
        //TODO: must change this
        //ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock));
        if (ChannelRequest<CHANNEL, REQUEST>::m_result != 0) {
            typename list<REQUEST>::iterator iter = ChannelRequest<CHANNEL, REQUEST>::m_result->begin();
            while (iter != ChannelRequest<CHANNEL, REQUEST>::m_result->end()) {
                (*(*iter))->removeListener();
                (*(*iter))->cancel();
                iter++;
            }
        }
        ChannelRequest<CHANNEL, REQUEST>::cancel(clearMap);
    }

    virtual list<REQUEST>* detach() {
        //ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, mon, *(ChannelRequest<CHANNEL, REQUEST>::m_lock), 0);
        if (m_debugRequestGroup) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: RequestGroup::detach() this=%@ !\n"), this));
        }
        typename list<REQUEST>::iterator iter = ChannelRequest<CHANNEL, REQUEST>::m_result->begin();
        while (iter != ChannelRequest<CHANNEL, REQUEST>::m_result->end()) {
            (*(*iter))->removeListener();
            //(*(*iter))->cancel();
            //(*(*iter))->cancel(true);
            //if(!(*(*iter))->isCancel()){
            (*(*iter))->close();
            //}
            iter++;
        }
        return ChannelRequest<CHANNEL, REQUEST>::detach();
    }

protected:
    bool m_debugRequestGroup;
    int m_finishedRequests;
    bool m_opened;

    bool isAllEventFinal() {
        return (ChannelRequest<CHANNEL, REQUEST>::m_expectedSize == m_finishedRequests);
    }

};

#endif	/* REQUESTGROUP_H */

