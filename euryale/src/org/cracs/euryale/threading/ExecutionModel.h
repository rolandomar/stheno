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
 * ExecutionModel.h
 *
 *  Created on: Jan 28, 2008
 *      Author: rmartins
 */

#ifndef EXECUTIONMODEL_H_
#define EXECUTIONMODEL_H_

#include <ace/Containers_T.h>
#include <ace/Thread_Manager.h>
#include <ace/Reactor.h>
#include <ace/Synch_Traits.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/Types.h>
#include <euryale/common/Object.h>
#include <euryale/net/EventHandler.h>

#include <euryale/event/Event.h>

#include <euryale/threading/exception/ExecutionModelException.h>
#include <euryale/threading/ExecutionModelResources.h>
#include <euryale/threading/ExecutionModelListener.h>

//Forward references
class CPUReservation;
class CPUQoS;

class ExecutionModel {
public:

    ExecutionModel() throw (ExecutionModelException&);
    ExecutionModel(ExecutionModelResources* resources) throw (ExecutionModelException&);
    virtual ~ExecutionModel();

    virtual void bind(EventHandler* channel) throw (ExecutionModelException&) = 0;

    virtual void unbind(EventHandler* channel) throw (ExecutionModelException&) = 0;

    //virtual void open(CPUReservation* reserve,CPUQoS* qos) throw (ExecutionModelException&) = 0;

    virtual void join(Event* event, ACE_Time_Value* timeout = 0) throw (ExecutionModelException&) = 0;

    void setExecutionModelListener(ExecutionModelListener* listener);

    virtual ExecutionModelResources& resources();

    virtual ExecutionModelResources* getResources() throw (ExecutionModelException&);

    /*{
       ACE_Countdown_Time tv(timeout);
       while(!tv.stopped() && event->keepWaiting()){
          
       }
    }*/

    virtual void shutdown() throw (ExecutionModelException&) = 0;

protected:
    ExecutionModelListener* m_ecListener;
    ExecutionModelResources* m_resources;

    void fireOnCloseEvent();

};

typedef ACE_Strong_Bound_Ptr<ExecutionModel, ACE_Recursive_Thread_Mutex> ExecutionModelPtr;

#endif /* EXECUTIONMODEL_H_ */
