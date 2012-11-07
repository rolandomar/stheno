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
 * File:   TPCTask.cpp
 * Author: rmartins
 * 
 * Created on December 9, 2009, 11:05 PM
 */


#include "TPCTask.h"

#include <euryale/threading/ExecutionFlowScope.h>

TPCTask::TPCTask(ThreadPerConnectionEnginePtr& tpc, ACE_Thread_Manager* manager) :
m_tpc(tpc),Task(0/*manager*/),m_closed(false) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::TPCTask() this=%@\n"),this));
}

TPCTask::~TPCTask() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::~TPCTask() this=%@\n"),this));
}

/*ACE_thread_t TPCTask::getHandle() {
    return m_handle;
}*/

int TPCTask::close(u_long flags) {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::close() this=%@\n"),this));  
    delete this;
    return 0;
    /*ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if(m_closed){
        return;        
    }
    m_closed = true;*/
    //m_tpc = 0;
}

int TPCTask::svc_i(void*) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - entered(%d) tpc=%@\n"), gettid(),m_tpc));
    //ExecutionFlowScope efs(m_tpc);
    //m_handle = ACE_OS::thr_self();
    while (true) {
        /*ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);
        if(m_closed){
            ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() -  closed EXIT(%d)\n"),gettid()));
            //delete this;
            return 0;
        }
        ace_mon.release();*/
        try {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - current em= %@\n"),ExecutionFlow::instance()->getCurrentExecutionModel()));                        
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - before join %d\n"),gettid()));            
            /*if(m_tpc == 0){
                ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - no tpc EXIT %d\n"),gettid()));
                delete this;
                return 0;
            }*/
            m_tpc->join();
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - after join %d\n"),gettid()));
        } catch (ExecutionModelException& ex) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() - JOIN EXCEPTION EXIT this=%@ %d\n"),this,gettid()));
            //delete this;
            return 0;
            //break;
        }
    }
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: TPCTask::svc() -  EXIT(%d)\n"),gettid()));
    //delete this;
    return 0;
}
