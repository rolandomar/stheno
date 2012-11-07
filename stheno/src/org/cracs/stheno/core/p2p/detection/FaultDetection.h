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
 * File:   FaultDetection.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 24, 2010, 2:58 PM
 */

#ifndef FAULTDETECTION_H
#define	FAULTDETECTION_H

#include <stheno/core/p2p/P2PService.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/detection/FaultListener.h>

//class Overlay;
class FaultDetection : public P2PService{
public:
    friend class Overlay;
    
    virtual ~FaultDetection();

    virtual void open(){
        this->toStart();
    }

    //this invalidates object
    virtual void close() {
        P2PService::close();
    }
    
    void removeFaultListener(FaultListener* listener);
    virtual void addFaultListener(FaultListener* listener);
    
    virtual const char* id(){
        return "C000000000000000000000000000000B";
    }
protected:
    list<FaultListener*> m_listeners;

    FaultDetection(Overlay* overlay);

    virtual void fireFaultEvent(PeerFault* e);
    
};

typedef ACE_Strong_Bound_Ptr<FaultDetection, ACE_Recursive_Thread_Mutex> FaultDetectionPtr;

#endif	/* FAULTDETECTION_H */

