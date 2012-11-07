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
 * File:   Overlay.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on April 29, 2010, 11:47 AM
 */

#ifndef OVERLAY_H
#define	OVERLAY_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/common/uuid/UUID.h>

//#include <stheno/core/SthenoCore.h>
#include <stheno/core/p2p/exception/OverlayException.h>
#include <stheno/service/exception/ServiceException.h>
#include <stheno/core/SthenoCoreInterface.h>
#include <stheno/core/p2p/discovery/Discovery.h>
#include <stheno/core/p2p/mesh/Mesh.h>
#include <stheno/core/p2p/ft/FaultTolerance.h>

//#include <stheno/core/p2p/network/Network.h>
//#include <stheno/core/p2p/detection/FaultDetection.h>

//Forward references
class SthenoCore;
class QoSManagerInterface;

class Overlay {
public:

    enum {
        NOT_INIT = 0x1,
        STARTING,
        RUNNING,
        SUSPENDING,
        SUSPENDED,
        RESUMING,
        CLOSING,
        CLOSED,
        ERROR                
    };

    virtual void open(QoSManagerInterface* qosManager) throw (OverlayException&);

    virtual void close() throw (OverlayException&);

    virtual void resume() throw (OverlayException&);        

    virtual void suspend() throw (OverlayException&);

    bool isValid();

    bool isSuspended();

    bool isClose();
    
    bool isStarting();
    
    bool isResuming();

    bool isNotInitialized();
    
    bool isInError();


    //friend class SthenoCore;
    virtual ~Overlay();
        
    virtual void setRuntime(SthenoCoreInterface* runtime);    
    virtual SthenoCoreInterface* getRuntime();
        
    //virtual DiscoveryPtr& getDiscovery() throw(ServiceException&) = 0;
    virtual void getDiscovery(DiscoveryPtr& discoveryPtr) throw(OverlayException&) = 0;
    virtual void getMesh(MeshPtr& meshPtr) throw(OverlayException&) = 0;    
    virtual void getFaultTolerance(FaultTolerancePtr& ftPtr) throw(OverlayException&) = 0;
    
    //virtual NetworkPtr& getNetwork() throw(ServiceException&) = 0;
    //virtual FaultDetectionPtr& getFaultDetection() throw(ServiceException&) = 0;
    
    virtual void getUUID(UUIDPtr& uuid);
    Byte getStatus();        
    ServiceClientPtr* getClient(UUIDPtr& sid,UUIDPtr& iid) throw(OverlayException&){
        throw OverlayException(OverlayException::PRIMITIVE_NOT_IMPLEMENTED);
    }

protected:
    ACE_SYNCH_RECURSIVE_MUTEX m_lock;    
    Byte m_status;    
    SthenoCoreInterface* m_runtime;    
    QoSManagerInterface* m_qosManager;
            
    Overlay(SthenoCoreInterface* runtime);
    Overlay();
            
    virtual QoSManagerInterface* getQoSManager();
    
    //State related
    virtual void open_i() throw (OverlayException&);
    
    virtual void close_i() throw (OverlayException&);   
    
    virtual void resume_i() throw (OverlayException&);
    
    virtual void suspend_i() throw (OverlayException&);
    
    void toStarting() throw (OverlayException&);    

    void toStart() throw (OverlayException&);

    void toSuspending() throw (OverlayException&);

    void toSuspended() throw (OverlayException&);

    void toResuming() throw (OverlayException&);

    void toClosing() throw (OverlayException&);

    void toClose() throw (OverlayException&);
    
    void toError() throw (OverlayException&);

    void changeState(int newState) throw (OverlayException&);
};

typedef ACE_Strong_Bound_Ptr<Overlay, ACE_Recursive_Thread_Mutex> OverlayPtr;

#endif	/* OVERLAY_H */

