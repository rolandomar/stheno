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
 * File:   EventHandler.h
 * Author: rmartins
 *
 * Created on December 14, 2009, 4:28 PM
 */

#ifndef _EVENTHANDLER_H
#define	_EVENTHANDLER_H

#include <ace/Event_Handler.h>
class EventHandler;
//#include <ace/Log_Msg.h>
//#include <euryale/Types.h>

class EventHandlerSpecialization {
public:

    virtual int handleInputImpl(EventHandler* handle, ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleInputImpl\n");
        return 0;
    }

    virtual int handleOutputImpl(EventHandler* handle, ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleOutputImpl\n");
        return 0;
    }

    virtual int handleExceptionImpl(EventHandler* handle, ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleExceptionImpl\n");
        return 0;
    }

    virtual int handleTimeoutImpl(EventHandler* handle, const ACE_Time_Value &current_time, const void *act = 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleTimeoutImpl\n");
        return 0;
    }

    virtual int handleExitImpl(EventHandler* handle, ACE_Process *p) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleExitImpl\n");
        return 0;
    }

    virtual int handleCloseImpl(EventHandler* handler, ACE_HANDLE handle, ACE_Reactor_Mask close_mask) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleCloseImpl\n");
        return 0;
    }

    virtual int handleSignalImpl(EventHandler* handle, int signum, siginfo_t *info = 0, ucontext_t *context = 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handleSignalImpl\n");
        return 0;
    }
};

class EventHandler : public ACE_Event_Handler {
public:

    EventHandler() : m_specializationImpl(0) {
    }

    virtual ~EventHandler() {
    }

    //virtual int open(void *arg = 0) = 0;

    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handle_input\n");
        //ACE_Time_Value start = ACE_OS::gettimeofday();
        notifyHandleInput(fd);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== handle_input thread_id:%lu\n",ACE_OS::thr_self());
        int ret = handleInput_i(fd);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== ret= %d handle_input thread_id:%lu\n",ret,ACE_OS::thr_self());
        //ACE_Time_Value end = ACE_OS::gettimeofday() - start;
        //    ULongLong deltausec;
        //    end.to_usec(deltausec);
        //    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)TRACE: EventHandler::handle_input() - time=%Q\n"), deltausec));
        return ret;
    }

    virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        notifyHandleOutput(fd);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)EventHandler::handle_output()")));
        int ret = handleOutput_i(fd);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== ret= %d handle_output %d\n",ret,ACE_OS::thr_self());
        return ret;
    }

    virtual int handle_exception(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== handle_exception %d\n",ACE_OS::thr_self());
        notifyHandleException(fd);
        return handleException_i(fd);
    }

    virtual int handle_timeout(const ACE_Time_Value &current_time,
                               const void *act = 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== handle_timeout %d\n",ACE_OS::thr_self());
        notifyHandleTimeout(current_time, act);
        return handleTimeout_i(current_time, act);
    }

    virtual int handle_exit(ACE_Process *p) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== handle_exit %d\n",ACE_OS::thr_self());
        notifyHandleExit(p);
        return handleExit_i(p);
    }

    virtual int handle_close(ACE_HANDLE handle,
                             ACE_Reactor_Mask close_mask) {
        /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: EventHandler::handle_close() - thread_id(%ld) handle(%d) mask(%ld)\n"),
                  ACE_OS::thr_self(), handle, close_mask));*/
        notifyHandleClose(handle, close_mask);
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)handle_close%d\n",ACE_OS::thr_self());
        return handleClose_i(handle, close_mask);
    }

    virtual int handle_signal(int signum, siginfo_t *info = 0, ucontext_t *context = 0) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=== handle_signal %d\n",ACE_OS::thr_self());
        notifyHandleSignal(signum, info, context);
        return handleSignal_i(signum, info, context);
    }


    /// Called when input events occur (e.g., connection or data).

    virtual int handleInput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        return 0;
    }

    virtual int handleOutput_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        return 0;
    }

    virtual int handleException_i(ACE_HANDLE fd = ACE_INVALID_HANDLE) {
        return 0;
    }

    virtual int handleTimeout_i(const ACE_Time_Value &current_time,
                                const void *act = 0) {
        return 0;
    }

    virtual int handleExit_i(ACE_Process *p) {
        return 0;
    }

    virtual int handleClose_i(ACE_HANDLE handle,
                              ACE_Reactor_Mask close_mask) {
        //delete this;
        //dont delete here
        return 0;
    }

    virtual int handleSignal_i(int signum, siginfo_t *info = 0, ucontext_t *context = 0) {
        return 0;
    }

    void attachSpecilization(EventHandlerSpecialization* impl) {
        m_specializationImpl = impl;
    }

protected:
    EventHandlerSpecialization* m_specializationImpl;
    //check threading strategy
    //if lf then we must elect a new leader

    virtual void notifyHandleOutput(ACE_HANDLE fd) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)notifyHandleOutput\n");
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleOutputImpl(this, fd);
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)end notifyHandleOutput\n");
    }

    //check threading strategy
    //if lf then we must elect a new leader

    virtual void notifyHandleInput(ACE_HANDLE fd) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)notifyHandleInput\n");
        if (m_specializationImpl != 0) {

            m_specializationImpl->handleInputImpl(this, fd);
        }
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)end notifyHandleInput\n");
    }

    virtual void notifyHandleException(ACE_HANDLE fd) {
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleExceptionImpl(this, fd);
        }
    }

    virtual void notifyHandleTimeout(const ACE_Time_Value &current_time,
                                     const void *act = 0) {
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleTimeoutImpl(this, current_time, act);
        }
    }

    virtual void notifyHandleExit(ACE_Process* p) {
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleExitImpl(this, p);
        }
    }

    virtual void notifyHandleClose(ACE_HANDLE handle,
                                   ACE_Reactor_Mask close_mask) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)PPPPPPPPPPPP\n");
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleCloseImpl(this, handle, close_mask);
        }
    }

    virtual void notifyHandleSignal(int signum, siginfo_t *info = 0,
                                    ucontext_t *context = 0) {
        if (m_specializationImpl != 0) {
            m_specializationImpl->handleSignalImpl(this, signum, info, context);
        }
    }


};

#endif	/* _EVENTHANDLER_H */

