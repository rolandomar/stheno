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
 * File:   ServiceAbstract.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 25, 2010, 10:04 AM
 */

#ifndef SERVICEABSTRACT_H
#define	SERVICEABSTRACT_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <euryale/Types.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
//Must a redo a lot of stuff for usign this...
//#include <euryale/common/sm/StateMachine.h>

#include <stheno/service/exception/ServiceException.h>
#include <stheno/service/ServiceParams.h>
#include <stheno/service/ReplicationMessage.h>
#include <euryale/net/endpoint/SAPInfo.h>

#include <stheno/service/ServiceQoSReservation.h>
#include <stheno/service/ServiceQoSResource.h>

class ReplicationGroup;
typedef ACE_Strong_Bound_Ptr<ReplicationGroup, ACE_SYNCH_MUTEX> ReplicationGroupPtr;

/**
 * Must have the interface to service's endpoints
 */
class ServiceListener;
class QoSManagerInterface;

class ServiceAbstract {
public:

    enum {
        NOT_INIT = 0x01,
        STARTING,
        RUNNING,
        SUSPENDING,
        SUSPENDED,
        FAULT,
        RECOVERING,
        CLOSING,
        CLOSED,
    };
    //  (initial)
    //  >NOT_INIT<  =>  STARTING
    //
    //  STARTING    =>  RUNNING
    //              =>  FAULT
    //              =>  CLOSING
    //
    //  RUNNING     =>  SUSPENDING
    //              =>  FAULT
    //              =>  CLOSING
    //
    //  FAULT       =>  RECOVERING    
    //              =>  CLOSING
    //  (recovering is intended for ft, in rt is a noop)
    //  RECOVERING  =>  RUNNING
    //              =>  CLOSING
    //
    //  CLOSING     =>  ->CLOSED<- (final)
    //
    //  SUSPENDING  =>  SUSPENDED
    //              =>  CLOSING
    //
    //  SUSPENDED   =>  STARTING
    //              =>  CLOSING
    //
    //FT role types

    enum {
        FT_NOT_SETTED = 0x00,
        FT_PRIMARY,
        FT_REPLICA
    };

    //Service types
    static const Byte P2P_TYPE = 0;
    static const Byte USER_TYPE = 1;

    ServiceAbstract(const char* id, UUIDPtr& iid, Byte type);
    ServiceAbstract(const char* id, Byte type);
    virtual ~ServiceAbstract();

    //QoS --->worst-case<--- resource consumption
    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params) = 0;
    virtual ServiceAbstract* duplicate() throw (ServiceException&) = 0;
    virtual const char* getName() = 0;
    //virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&) = 0;
    //virtual SAPInfoPtr& getEndpoints() throw (ServiceException&) = 0;
    virtual void getSAP(SAPInfoPtr& s) = 0;
    virtual const char* id() = 0;

    //User or P2P
    Byte getType();
    // is this a low level p2p service?
    bool isP2PService();
    //is this a user high level service?
    bool isUserService();

    //service ID
    const void getSID(UUIDPtr& sid);
    //instance ID
    const void getIID(UUIDPtr& uuid);
    //must be overridden
    void setIID(UUIDPtr& iid);
    //void open();
    virtual void open(ServiceParamsPtr& params, int ftType,QoSManagerInterface* qosManager) throw (ServiceException&);

    void close() throw (ServiceException&);

    void resume() throw (ServiceException&);

    void suspend() throw (ServiceException&);

    //something triggered a fault in the system and provoked a fault in this service
    void fault() throw (ServiceException&);

    bool isValid();

    bool isSuspending();

    bool isSuspended();

    bool isInFault();

    bool isClose();

    bool isClosing();

    bool isStarting();

    bool isRecovering();

    Byte getStatus();

    void addServiceListener(ServiceListener* serviceListener);

    virtual bool changeToPrimaryRole();
    virtual bool isReplica();
    virtual bool isPrimary();
    virtual bool isFT();
    
    virtual QoSManagerInterface* getQoSManager();

    
    virtual void onReplication(ACE_Message_Block* replicationData) throw (ServiceException&);
    void setReplicationGroup(ReplicationGroupPtr& rg);
    virtual void replicate(ACE_Message_Block* mb) throw (ServiceException&);
    
protected:
    UUIDPtr m_sid;
    UUIDPtr m_iid;
    Byte m_type;
    Byte m_status;
    list<ServiceListener*> m_listeners;
    QoSManagerInterface* m_qosManager;

    ACE_SYNCH_RECURSIVE_MUTEX m_lock;
    ACE_SYNCH_RECURSIVE_MUTEX& getLock();

    int m_ftType;
    //FT Interface
            
    virtual void setAsPrimary() throw (ServiceException&) {
    }

    /*virtual void onReplication(ReplicationMessage* msg) throw (ServiceException&) {
    }*/       

    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) = 0;

    virtual void close_i() throw (ServiceException&) {
    }

    virtual void resume_i() throw (ServiceException&) {
    }

    virtual void suspend_i() throw (ServiceException&) {
    }

    virtual void fault_i() throw (ServiceException&) {
    }

    virtual void recovering_i() throw (ServiceException&) {
    }

    void toStarting() throw (ServiceException&);

    void toStart() throw (ServiceException&);

    void toSuspending() throw (ServiceException&);

    void toSuspended() throw (ServiceException&);

    void toFault() throw (ServiceException&);

    void toClosing() throw (ServiceException&);

    void toClose() throw (ServiceException&);

    void toRecovering() throw (ServiceException&);

    void changeState(int newState) throw (ServiceException&);

    void handleNotInitState(int newState) throw (ServiceException&);
    void handleStartingState(int newState) throw (ServiceException&);
    void handleRunningState(int newState) throw (ServiceException&);
    void handleSuspendingState(int newState) throw (ServiceException&);
    void handleSuspendedState(int newState) throw (ServiceException&);
    void handleFaultState(int newState) throw (ServiceException&);
    void handleClosingState(int newState) throw (ServiceException&);
    void handleClosedState(int newState) throw (ServiceException&);
    void handleRecoveringState(int newState) throw (ServiceException&);



private:
    ReplicationGroupPtr m_rg;
    ServiceAbstract(const ServiceAbstract& orig);
};

typedef ACE_Strong_Bound_Ptr<ServiceAbstract, ACE_SYNCH_MUTEX> ServiceAbstractPtr;

#endif	/* SERVICEABSTRACT_H */

