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
 * File:   P2PService.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on April 29, 2010, 12:03 PM
 */

#include "P2PService.h"
#include <stheno/core/p2p/Overlay.h>

P2PService::P2PService(const char* id, Overlay* overlay) :
ServiceAbstract(id, ServiceAbstract::P2P_TYPE), m_overlay(overlay) {
}

P2PService::~P2PService() {
    //delete m_overlay;
}

void P2PService::close_i() throw (ServiceException&) {
    m_overlay = 0;
}

//not applicable to P2P services

ServiceAbstract* P2PService::duplicate() throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

Overlay* P2PService::getOverlay() {
    return m_overlay;
}