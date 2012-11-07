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
 * File:   ExecutionModelListener.h
 * Author: rmartins
 *
 * Created on January 10, 2011, 6:28 PM
 */

#ifndef EXECUTIONMODELLISTENER_H
#define	EXECUTIONMODELLISTENER_H

#include <euryale/common/Object.h>

class ExecutionModel;
class ExecutionModelListener : public Object {
public:
    ExecutionModelListener();
    virtual ~ExecutionModelListener();

    virtual void onShutdown(ExecutionModel* ec);
    virtual void onExecutionModelChanged(int newModel);
    virtual const char* objectID() const;
};


#endif	/* EXECUTIONMODELLISTENER_H */

