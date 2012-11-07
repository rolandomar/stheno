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
 * File:   LeaderFollowers.h
 * Author: rmartins
 *
 * Created on December 14, 2009, 6:24 PM
 */

#ifndef _LEADERFOLLOWERS_H
#define	_LEADERFOLLOWERS_H

#include <ace/Lock.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_time.h>
#include <ace/Task.h>
#include <ace/Containers.h>
#include <ace/Synch.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Impl.h>
#include <ace/TP_Reactor.h>


#include <euryale/threading/tp/ThreadPool.h>
#include <euryale/event/Event.h>
#include <euryale/common/Synchronizable.h>

class Follower: public Synchronizable,public EventListener {
public:

    Follower(ACE_SYNCH_RECURSIVE_MUTEX& leaderLock);
    ~Follower();
    

    /*int wait(ACE_Time_Value* timeout = 0);

    int signal(void);*/

    virtual void onEvent(Event* event, int oldState, int newState){
        ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, leader_mon, lock());
        this->signal();
    }

    ACE_thread_t owner(void);

protected:
    
    //ACE_Condition<ACE_SYNCH_RECURSIVE_MUTEX> m_cond;
    ACE_thread_t m_owner;
};

class LFThreadPool : public ThreadPool, EventHandlerSpecialization {
public:

    LFThreadPool(ExecutionModelResources* resources) throw(ExecutionModelException&);
    LFThreadPool() throw(ExecutionModelException&);
    virtual ~LFThreadPool();
    //virtual int svc(void);

    virtual void join(Event* event = 0, ACE_Time_Value* timeout = 0) throw(ExecutionModelException&);

    int electNewLeader(void);

    virtual void bind(EventHandler* eh) throw(ExecutionModelException&);
    virtual void unbind(EventHandler* channel) throw(ExecutionModelException&){}
    
    virtual int handleInputImpl(EventHandler* handle, ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handleOutputImpl(EventHandler* handle, ACE_HANDLE fd = ACE_INVALID_HANDLE);
    virtual int handleCloseImpl(EventHandler* handler, ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    virtual int handleExceptionImpl(EventHandler* handle,ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        electNewLeader();
    return 0;
    }
    virtual int handleTimeoutImpl(EventHandler* handle,const ACE_Time_Value &current_time,const void *act = 0) {
        electNewLeader();
    return 0;
    }
    virtual int handleExitImpl(EventHandler* handle,ACE_Process *p) {
        electNewLeader();
    return 0;
    }

    virtual int handleSignalImpl(EventHandler* handle,int signum, siginfo_t *info = 0, ucontext_t *context = 0) {
        electNewLeader();
    return 0;
    }

    void open(UInt staticTasksNo, UInt maxDynamicTasks) throw(ExecutionModelException&);
    //default entry
    void open() throw(ExecutionModelException&){
        open(2,0);
    }
    ACE_Reactor* getReactor();

protected:

    int becomeLeader(Event* event, ACE_Time_Value* timeout);

    Follower* addFollower(void);
    void addFollower(Follower* fw);
    //timeout situation
    bool removeFollower(Follower *fw);

    int isLeaderActive(void);

    void setLeaderActive(ACE_thread_t leader);

    void processMessage(ACE_Message_Block *mb);

    bool isEventFinish(Event* event);

    bool isThreadLeader(ACE_thread_t handle);
    
    volatile ACE_thread_t m_currentLeader;
    //ACE_Thread_Mutex m_leaderLock;
    ACE_SYNCH_RECURSIVE_MUTEX m_leaderLock;
    //ACE_Unbounded_Queue<Follower*> m_followers;
    list<Follower*> m_followers;
    ACE_SYNCH_RECURSIVE_MUTEX m_followersLock;
    //ACE_SYNCH_RECURSIVE_MUTEX m_followersLock;
    static long LONG_TIME;

    bool isTimeout(ACE_Time_Value* timeout);


    
    ACE_Reactor* allocateReactorImpl(void) const;

    void printFollowers(){
        list<Follower*>::iterator iter = m_followers.begin();
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Followers[")));
        while(iter!= m_followers.end()){
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)%lu,"),(*iter)->owner()));
            iter++;
        }
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)]\n")));
    }
    int counter;

};

#endif	/* _LEADERFOLLOWERS_H */

