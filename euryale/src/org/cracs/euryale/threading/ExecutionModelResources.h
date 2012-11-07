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
 * File:   ExecutionModelResources.h
 * Author: rmartins
 *
 * Created on January 10, 2011, 6:24 PM
 */

#ifndef EXECUTIONMODELRESOURCES_H
#define	EXECUTIONMODELRESOURCES_H

#include <ace/Reactor.h>

class ExecutionModelResources {
public:

    ExecutionModelResources();
    ExecutionModelResources(ACE_Reactor* reactor, bool dynamicReactor = true);
    virtual ~ExecutionModelResources();

    virtual ACE_Reactor* getReactor();
    virtual void setReactor(ACE_Reactor* reactor, bool dynamicReactor = true);

protected:
    ACE_Reactor* m_reactor;
    bool m_dynamicReactor;
};


#endif	/* EXECUTIONMODELRESOURCES_H */

