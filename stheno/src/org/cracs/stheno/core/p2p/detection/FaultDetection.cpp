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
 * File:   FaultDetection.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 24, 2010, 2:58 PM
 */

#include "FaultDetection.h"
#include <stheno/core/p2p/Overlay.h>

FaultDetection::FaultDetection(Overlay* overlay):P2PService(id(),overlay) {
}


FaultDetection::~FaultDetection() {
}

void FaultDetection::addFaultListener(FaultListener* listener){
    m_listeners.push_back(listener);
}

void FaultDetection::removeFaultListener(FaultListener* listener){
    m_listeners.remove(listener);
}

void FaultDetection::fireFaultEvent(PeerFault* e){
    list<FaultListener*>::iterator iter = m_listeners.begin();
    while(iter != m_listeners.end()){
        (*iter)->onPeerFailure(e);
        iter++;
    }
}