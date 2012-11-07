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
 * File:   FaultTolerance.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 21, 2010, 11:14 AM
 */

#include "FaultTolerance.h"

#include <stheno/service/UserService.h>
#include <stheno/core/p2p/Overlay.h>
#include <stheno/service/FTServiceParams.h>
#include <stheno/core/p2p/ft/ReplicationGroup.h>

FaultTolerance::FaultTolerance(Overlay* overlay) : P2PService(id(), overlay) {
}

FaultTolerance::~FaultTolerance() {
}

const char* FaultTolerance::id() {
    return "B501978E8295DCB672C6B9FAF04D370A";
}
