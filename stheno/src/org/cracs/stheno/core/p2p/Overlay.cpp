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
 * File:   Overlay.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 29, 2010, 11:47 AM
 */

#include "Overlay.h"
#include <stheno/core/SthenoCoreInterface.h>
#include <stheno/core/p2p/discovery/Discovery.h>
#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/SthenoCore.h>

Overlay::Overlay() :
m_runtime(0), m_status(NOT_INIT) {
}

Overlay::Overlay(SthenoCoreInterface* runtime) :
m_runtime(runtime), m_status(NOT_INIT) {
}

Overlay::~Overlay() {

}

Byte Overlay::getStatus() {
    return m_status;
}

void Overlay::setRuntime(SthenoCoreInterface* runtime) {
    m_runtime = runtime;    
}

QoSManagerInterface* Overlay::getQoSManager(){
    return m_qosManager;
}


void Overlay::open(QoSManagerInterface* qosManager) throw (OverlayException&){
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);    
    toStarting();    
    m_qosManager= qosManager;
    open_i();
    toStart();
}

void Overlay::close() throw (OverlayException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toClosing();

    close_i();
    toClose();
}

void Overlay::resume() throw (OverlayException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toResuming();
    resume_i();
    toStart();
}

void Overlay::suspend() throw (OverlayException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toSuspending();
    suspend_i();
    toSuspended();
}

bool Overlay::isValid() {
    return (m_status == RUNNING);
}

bool Overlay::isSuspended() {
    return (m_status == SUSPENDED);
}

bool Overlay::isClose() {
    return (m_status == CLOSED);
}

bool Overlay::isStarting() {
    return (m_status == STARTING);
}

bool Overlay::isResuming() {
    return (m_status == RESUMING);
}

bool Overlay::isInError() {
    return (m_status == ERROR);
}

bool Overlay::isNotInitialized() {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)isNotInitialized(): status(%d) NOT_INIT(%d)\n"), m_status, NOT_INIT));
    return (m_status == NOT_INIT);
}

SthenoCoreInterface* Overlay::getRuntime(){
    return m_runtime;
}

void Overlay::getUUID(UUIDPtr& uuid) {
    getRuntime()->getUUID(uuid);
}


void Overlay::open_i() throw (OverlayException&) {
}

void Overlay::close_i() throw (OverlayException&) {
    m_runtime = 0;
}

void Overlay::resume_i() throw (OverlayException&) {
}

void Overlay::suspend_i() throw (OverlayException&) {
}

void Overlay::toStarting() throw (OverlayException&) {
    changeState(STARTING);
}

void Overlay::toStart() throw (OverlayException&) {
    changeState(RUNNING);
}

void Overlay::toSuspending() throw (OverlayException&) {
    changeState(SUSPENDING);
}

void Overlay::toSuspended() throw (OverlayException&) {
    changeState(SUSPENDED);
}

void Overlay::toResuming() throw (OverlayException&) {
    changeState(RESUMING);
}

void Overlay::toClosing() throw (OverlayException&) {
    changeState(CLOSING);
}

void Overlay::toClose() throw (OverlayException&) {
    changeState(CLOSED);
}

void Overlay::toError() throw (OverlayException&) {
    changeState(ERROR);
}

void Overlay::changeState(int newState) throw (OverlayException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    m_status = newState;
}
