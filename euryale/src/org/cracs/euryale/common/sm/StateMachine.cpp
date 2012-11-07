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
 * File:   StateMachine.cpp
 * Author: rmartins
 * 
 * Created on August 3, 2010, 4:09 PM
 */

#include "StateMachine.h"

StateMachine::StateMachine():m_status(NOT_INIT){
}

StateMachine::StateMachine(const StateMachine& orig) {
    m_status = orig.m_status;
}

StateMachine::~StateMachine() {
}

void StateMachine::open() throw (StateMachineException&){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toStarting();
    open_i();
    toStart();
}

void StateMachine::close() throw (StateMachineException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toClosing();
    close_i();
    toClose();
}

void StateMachine::resume() throw (StateMachineException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toResuming();
    resume_i();
    toStart();
}

void StateMachine::suspend() throw (StateMachineException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toSuspending();
    suspend_i();
    toSuspended();
}

bool StateMachine::isValid() {
    return (m_status == RUNNING);
}

bool StateMachine::isSuspended() {
    return (m_status == SUSPENDED);
}

bool StateMachine::isClose() {
    return (m_status == CLOSED);
}

bool StateMachine::isStarting() {
    return (m_status == STARTING);
}


Byte StateMachine::getStatus() {
    return m_status;
}

void StateMachine::toStarting() throw (StateMachineException&) {
    changeState(STARTING);
}

void StateMachine::toStart() throw (StateMachineException&) {
    changeState(RUNNING);
}

void StateMachine::toSuspending() throw (StateMachineException&) {
    changeState(SUSPENDING);
}

void StateMachine::toSuspended() throw (StateMachineException&) {
    changeState(SUSPENDED);
}

void StateMachine::toResuming() throw (StateMachineException&) {
    changeState(RESUMING);
}

void StateMachine::toClosing() throw (StateMachineException&) {
    changeState(CLOSING);
}

void StateMachine::toClose() throw (StateMachineException&) {
    changeState(CLOSED);
}

void StateMachine::changeState(int newState) throw (StateMachineException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_status = newState;
}