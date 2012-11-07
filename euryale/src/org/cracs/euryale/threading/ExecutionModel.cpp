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
 * ExecutionModel.cpp
 *
 *  Created on: Jan 28, 2008
 *      Author: rmartins
 */

#include "ExecutionModel.h"
#include <euryale/threading/ExecutionFlow.h>

ExecutionModel::ExecutionModel() throw (ExecutionModelException&) {
    m_resources = new ExecutionModelResources();
}

ExecutionModel::~ExecutionModel() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ExecutionModel::~ExecutionModel()\n")));
    //shutdown();
    delete m_resources;
}

ExecutionModel::ExecutionModel(ExecutionModelResources* resources) throw (ExecutionModelException&) :
m_resources(resources) {
}

void ExecutionModel::setExecutionModelListener(ExecutionModelListener* listener) {
    m_ecListener = listener;
}

ExecutionModelResources& ExecutionModel::resources() {
    return *m_resources;
}

ExecutionModelResources* ExecutionModel::getResources() throw (ExecutionModelException&) {
    return m_resources;
}

void ExecutionModel::fireOnCloseEvent() {
    if (m_ecListener != 0) {
        m_ecListener->onShutdown(this);
    }
}