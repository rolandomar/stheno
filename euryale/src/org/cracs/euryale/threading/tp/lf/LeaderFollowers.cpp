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
 * File:   LeaderFollowers.cpp
 * Author: rmartins
 * 
 * Created on December 14, 2009, 6:24 PM
 */

#include "LeaderFollowers.h"

Follower::Follower(ACE_SYNCH_RECURSIVE_MUTEX& leaderLock) 
: Synchronizable(leaderLock) {//m_cond(leaderLock) {
    m_owner = ACE_Thread::self();
}

Follower::~Follower() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)~Follower %d\n",m_owner);
}

/*int Follower::wait(ACE_Time_Value* timeout) {
    return this->m_cond.wait(timeout);
}

int Follower::signal(void) {
    return this->m_cond.signal();
}*/

ACE_thread_t Follower::owner(void) {
    return this->m_owner;
}

LFThreadPool::LFThreadPool(ExecutionModelResources* resources) throw(ExecutionModelException&):
ThreadPool(resources), m_currentLeader(0) {
    counter = 0;
}

LFThreadPool::LFThreadPool() throw(ExecutionModelException&):  
m_currentLeader(0) {
    counter = 0;
    m_resources->setReactor(allocateReactorImpl(), true);

}

LFThreadPool::~LFThreadPool() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Followers size=%d\n", m_followers.size());
}

int LFThreadPool::isLeaderActive(void) {
    return this->m_currentLeader != 0;
}

void LFThreadPool::setLeaderActive(ACE_thread_t leader) {
    this->m_currentLeader = leader;
}

void LFThreadPool::bind(EventHandler* eh) throw(ExecutionModelException&) {
    eh->attachSpecilization(this);
    eh->reactor(resources().getReactor());
    //TODO: must check this!!
    /*if (eh->open() == -1) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ThreadPerConnection::open:eh failed\n");
        throw(ExecutionModelException(ExecutionModelException::BAD_BINDING));
    }*/
}

int LFThreadPool::handleInputImpl(EventHandler* handle, ACE_HANDLE fd) {
    electNewLeader();
    return 0;
}

int LFThreadPool::handleOutputImpl(EventHandler* handle, ACE_HANDLE fd) {
    electNewLeader();
    return 0;
}

int LFThreadPool::handleCloseImpl(EventHandler* handler, ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
    electNewLeader();
    return 0;
}


void
LFThreadPool::join(Event* event, ACE_Time_Value* timeout) throw(ExecutionModelException&){
    int ret = 0;
    do {
        if (timeout != 0) {
            ACE_Time_Value now = ACE_OS::gettimeofday();
            if (now > *timeout) {
                //return -1;
                throw(ExecutionModelException(ExecutionModelException::TIMEOUT));
            }
        }        
        ret = becomeLeader(event, timeout); // Block until this thread is the leader.
        if (ret == -1) {
            if (m_currentLeader == ACE_OS::thr_self()) {
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ERROR LEADER MUST BE ELECTED\n");
                electNewLeader();
            }
            //return -1;
            throw(ExecutionModelException::ERROR_BECOMING_LEADER);
        }
        if (event != 0) {
            if (!event->keepWaiting()) {
                if (m_currentLeader == ACE_OS::thr_self()) {
                    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)EVNT RETURN ELECT LEADER\n");
                    if(electNewLeader() != 0){
                        throw(ExecutionModelException::ERROR_ELECTING_LEADER);
                    }
                }
                //return 0;
                return;
            }
        }

        if (ret == -1) {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Error: After wait to be leader %lu %lu tid=%d\n"), ACE_OS::thr_self(), m_currentLeader, gettid()));
            //return -1;
            throw(ExecutionModelException::ERROR_ELECTING_LEADER);
        }

        resources().getReactor()->owner(ACE_OS::thr_self());
        ret = resources().getReactor()->handle_events();
        if (m_currentLeader == ACE_OS::thr_self()) {
            electNewLeader();
        }

    } while (event != 0 && event->keepWaiting());

    //return 0;    
    return;
}

int LFThreadPool::becomeLeader(Event* event, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, leader_mon, this->m_leaderLock, -1);
    if (isLeaderActive()) {
        Follower *fw = addFollower();

        if (fw == 0) {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)error!!!!\n")));
            return -1;
        }
        if (event != 0) {
            event->setListener(fw);
        }

        //checks if we are the leader, or until timeout happens
        while (isLeaderActive() && !(isThreadLeader(fw->owner()))) {
            if (fw->wait(timeout) == -1) {
                ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)LFThreadPool: AFTER WAITTIMEUOT currentLeader=%lu self=%lu self_tid=%d\n"),
                                             m_currentLeader, ACE_OS::thr_self(), gettid()));
                removeFollower(fw);
                //we could be the leader
                ACE_thread_t owner = fw->owner();
                if (event != 0) {
                    event->removeListener();
                }
                delete fw;
                if (owner == m_currentLeader) {
                    //elect new leader                    
                    return this->electNewLeader();
                }
                //not leader, just return
                return -1;
            }
            if (event != 0) {
                if (!event->keepWaiting()) {
                    if (isThreadLeader(fw->owner())) {
                        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)LFThreadPool: Leader EVENT DONE currentLeader=%lu self=%lu self_tid=%lu\n", m_currentLeader, ACE_OS::thr_self(), gettid());
                        if (event != 0) {
                            event->removeListener();
                        }
                        removeFollower(fw);
                        delete fw;
                        return this->electNewLeader();
                    } else {
                        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)LFThreadPool: NOT LEADER EVENT DONE currentLeader=%lu self=%lu self_tid=%lu\n", m_currentLeader, ACE_OS::thr_self(), gettid());
                        if (event != 0) {
                            event->removeListener();
                        }
                        removeFollower(fw);
                        delete fw;
                        return 0;
                    }
                }
            }
        }
        if (event != 0) {
            event->removeListener();
        }
        delete fw;
    } else {
        //no followers
        setLeaderActive(ACE_Thread::self());
    }
    return 0;
}

Follower* LFThreadPool::addFollower(void) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)make_follower %d\n"), gettid()));
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, follower_mon, this->m_followersLock, 0);
    Follower *fw = 0;
    ACE_NEW_RETURN(fw, Follower(this->m_leaderLock), 0);

    m_followers.push_back(fw);
    return fw;
}

void LFThreadPool::addFollower(Follower* fw) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, follower_mon, this->m_followersLock);
    m_followers.push_back(fw);
}

bool LFThreadPool::removeFollower(Follower *fw) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, follower_mon, this->m_followersLock, false);
    list<Follower*>::iterator iter;
    iter = m_followers.begin();
    while (iter != m_followers.end()) {
        if (fw == *iter) {
            iter = this->m_followers.erase(iter);
            return true;
        }
        iter++;
    }

    return false;
}

int LFThreadPool::electNewLeader(void) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, leader_mon, this->m_leaderLock, -1);
    if (ACE_OS::thr_self() != this->m_currentLeader) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)electNewLeader: not leader error self=%lu currentLeader=%lu\n"), ACE_OS::thr_self(), m_currentLeader));
        return 0;
    }

    setLeaderActive(0);
    if (!m_followers.empty()) {
        Follower *fw = 0;
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, leader_mon, this->m_leaderLock, -1);
            fw = m_followers.front();
            removeFollower(fw);
        }
        if (fw == 0) {
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Error!\n")));
            return -1;
        }
        setLeaderActive(fw->owner());
        int ret = fw->signal();
        if (ret != 0) {
            setLeaderActive(0);
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)VVVVVVVV Error(2)!\n")));
            return electNewLeader();
        }
        return 0;
    } else {
        setLeaderActive(0);
    }
    return 0;
}

void
LFThreadPool::processMessage(ACE_Message_Block *mb) {
    if (mb == 0) {
        return;
    }
    int msgId;
    ACE_OS::memcpy(&msgId, mb->rd_ptr(), sizeof (int));
    mb->release();

    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T)(%t) Started processing message:%d\n"),
            msgId));
    ACE_OS::sleep(1);
    ACE_DEBUG((LM_DEBUG,
            ACE_TEXT("(%T)(%t) Finished processing message:%d\n"),
            msgId));
}

void LFThreadPool::open(UInt staticTasksNo, UInt maxDynamicTasks) throw(ExecutionModelException&) {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LFThreadPool::open(%d,%d)\n"), staticTasksNo, maxDynamicTasks));
    ThreadPool::open(staticTasksNo, maxDynamicTasks);
}

bool LFThreadPool::isTimeout(ACE_Time_Value* timeout) {
    if (timeout == 0) {
        return false;
    }
    if (*timeout > ACE_OS::gettimeofday()) {
        return true;
    }
    return false;
}

bool LFThreadPool::isEventFinish(Event* event) {
    //No event => always finished
    if (event == 0) {
        return true;
    }
    return event->keepWaiting();
}

bool LFThreadPool::isThreadLeader(ACE_thread_t handle) {
    return (handle == m_currentLeader);
}

ACE_Reactor* LFThreadPool::allocateReactorImpl(void) const {
    ACE_Reactor_Impl *impl = 0;
    /*
     * Hook to specialize RTo's reactor implementation.
     */
    ACE_NEW_RETURN(impl,
            ACE_TP_Reactor(ACE::max_handles(),
            1,
            (ACE_Sig_Handler*) 0,
            (ACE_Timer_Queue*) 0,
            0,
            ACE_Select_Reactor_Token::FIFO),
            0);
    return (ACE_Reactor*) impl;
}


long LFThreadPool::LONG_TIME = 5L;

