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
 * File:   ServiceAbstract.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on June 25, 2010, 10:04 AM
 */

#include "ServiceAbstract.h"
#include <stheno/service/ServiceListener.h>
#include <stheno/core/SthenoCoreInterface.h>

#include <stheno/core/p2p/ft/ReplicationGroup.h>

ServiceAbstract::ServiceAbstract(const char* id, UUIDPtr& iid, Byte type) :
m_sid(new UUID(id)),
m_iid(iid),
m_type(type),
m_status(NOT_INIT) {
}

ServiceAbstract::ServiceAbstract(const char* id, Byte type) :
m_sid(new UUID(id)),
m_iid(UUID::generateUUID()),
m_type(type),
m_status(NOT_INIT) {
}

ServiceAbstract::ServiceAbstract(const ServiceAbstract& orig) {
    //not implemented
}

ServiceAbstract::~ServiceAbstract() {
}

const void ServiceAbstract::getSID(UUIDPtr& sid) {
    sid = m_sid;
}

const void ServiceAbstract::getIID(UUIDPtr& iid) {
    iid = m_iid;
}

void ServiceAbstract::setIID(UUIDPtr& iid) {
    m_iid = iid;
}

Byte ServiceAbstract::getType() {
    return m_type;
}

bool ServiceAbstract::isP2PService() {
    return (m_type == P2P_TYPE);
}

bool ServiceAbstract::isUserService() {
    return !isP2PService();
}

void ServiceAbstract::addServiceListener(ServiceListener* serviceListener) {
    list<ServiceListener*>::iterator iter = m_listeners.begin();
    bool isPresent = false;
    while (iter != m_listeners.end()) {
        if (*iter == serviceListener) {
            isPresent = true;
        }
        iter++;
    }
    if (isPresent) {
        return;
    }
    m_listeners.push_back(serviceListener);
}

QoSManagerInterface* ServiceAbstract::getQoSManager() {
    return m_qosManager;
}

void ServiceAbstract::open(ServiceParamsPtr& params, int ftype, QoSManagerInterface* qosManager) throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toStarting();
    m_qosManager = qosManager;
    open_i(params, ftype);
    toStart();
}

void ServiceAbstract::close() throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toClosing();
    close_i();
    toClose();
}

void ServiceAbstract::resume() throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toStarting();
    resume_i();
    toStart();
}

void ServiceAbstract::suspend() throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toSuspending();
    suspend_i();
    toSuspended();
}

void ServiceAbstract::fault() throw (ServiceException&) {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    toFault();
    fault_i();
}

Byte ServiceAbstract::getStatus() {
    return m_status;
}

ACE_SYNCH_RECURSIVE_MUTEX& ServiceAbstract::getLock() {
    return m_lock;
}

bool ServiceAbstract::isValid() {
    return (m_status == RUNNING);
}

bool ServiceAbstract::isInFault() {
    return (m_status == FAULT);
}

bool ServiceAbstract::isSuspending() {
    return (m_status == SUSPENDING);
}

bool ServiceAbstract::isSuspended() {
    return (m_status == SUSPENDED);
}

bool ServiceAbstract::isClosing() {
    return (m_status == CLOSING);
}

bool ServiceAbstract::isClose() {
    return (m_status == CLOSED);
}

bool ServiceAbstract::isStarting() {
    return (m_status == STARTING);
}

bool ServiceAbstract::isRecovering() {
    return (m_status == RECOVERING);
}

void ServiceAbstract::toStarting() throw (ServiceException&) {
    changeState(STARTING);
}

void ServiceAbstract::toStart() throw (ServiceException&) {
    changeState(RUNNING);
}

void ServiceAbstract::toSuspending() throw (ServiceException&) {
    changeState(SUSPENDING);
}

void ServiceAbstract::toSuspended() throw (ServiceException&) {
    changeState(SUSPENDED);
}

void ServiceAbstract::toFault() throw (ServiceException&) {
    changeState(FAULT);
}

void ServiceAbstract::toClosing() throw (ServiceException&) {
    changeState(CLOSING);
}

void ServiceAbstract::toClose() throw (ServiceException&) {
    changeState(CLOSED);
}

void ServiceAbstract::toRecovering() throw (ServiceException&) {
    changeState(RECOVERING);
}

void ServiceAbstract::changeState(int newState) throw (ServiceException&) {
    printf("ServiceAbstract::changeState(%d,%d\n", newState, m_status);
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    switch (m_status) {
        case NOT_INIT:
        {
            handleNotInitState(newState);
            break;
        }
        case STARTING:
        {
            handleStartingState(newState);
            break;
        }
        case RUNNING:
        {
            handleRunningState(newState);
            break;
        }
        case SUSPENDING:
        {
            handleSuspendingState(newState);
            break;
        }
        case SUSPENDED:
        {
            handleSuspendedState(newState);
            break;
        }
        case FAULT:
        {
            handleFaultState(newState);
            break;
        }
        case RECOVERING:
        {
            handleRecoveringState(newState);
            break;
        }
        case CLOSING:
        {
            handleClosingState(newState);
            break;
        }
        case CLOSED:
        {
            handleClosedState(newState);
            break;
        }
        default:
            throw ServiceException(ServiceException::INSUFFICIENT_RESOURCES);
    }
    m_status = newState;
}

void ServiceAbstract::handleNotInitState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case FAULT:
        case CLOSING:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case STARTING:
            break;
    }
}

void ServiceAbstract::handleStartingState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case STARTING:
        case SUSPENDING:
        case SUSPENDED:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case RUNNING:
        case FAULT:
        case CLOSING:
            break;
    }
}

void ServiceAbstract::handleRunningState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case STARTING:
        case RUNNING:
        case SUSPENDED:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case SUSPENDING:
        case FAULT:
        case CLOSING:
            break;
    }
}

void ServiceAbstract::handleSuspendingState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case STARTING:
        case RUNNING:
        case SUSPENDING:
        case FAULT:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case SUSPENDED:
        case CLOSING:
            break;
    }
}

void ServiceAbstract::handleSuspendedState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case FAULT:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case STARTING:
        case CLOSING:
            break;
    }
}

void ServiceAbstract::handleFaultState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case FAULT:
        case RECOVERING:
        case CLOSING:
            break;
    }
}

void ServiceAbstract::handleClosingState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case STARTING:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case FAULT:
        case CLOSING:
        case RECOVERING:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
        case CLOSED:
            break;
    }
}

void ServiceAbstract::handleClosedState(int newState) throw (ServiceException&) {
    switch (newState) {
        case NOT_INIT:
        case STARTING:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case FAULT:
        case CLOSING:
        case CLOSED:
        case RECOVERING:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
    }
}

void ServiceAbstract::handleRecoveringState(int newState) throw (ServiceException&) {
    switch (newState) {
        case STARTING:
        case CLOSING:
        case RECOVERING:
            break;
        case NOT_INIT:
        case RUNNING:
        case SUSPENDING:
        case SUSPENDED:
        case FAULT:
        case CLOSED:
        default:
            throw ServiceException(ServiceException::INVALID_STATE);
    }
}

bool ServiceAbstract::changeToPrimaryRole() {
    return false;
}

bool ServiceAbstract::isReplica() {
    return (m_ftType == FT_REPLICA);
}

bool ServiceAbstract::isPrimary() {
    return (m_ftType == FT_PRIMARY);
}

bool ServiceAbstract::isFT() {
    return (m_ftType != FT_NOT_SETTED);
}

void ServiceAbstract::onReplication(ACE_Message_Block* replicationData) throw (ServiceException&) {
}

void ServiceAbstract::setReplicationGroup(ReplicationGroupPtr& rg) {
    m_rg = rg;
}

void ServiceAbstract::replicate(ACE_Message_Block* mb) throw (ServiceException&) {
    m_rg->replicate(mb);
    
}
