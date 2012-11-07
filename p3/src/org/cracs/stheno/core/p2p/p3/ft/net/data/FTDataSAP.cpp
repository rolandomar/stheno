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
 * File:   FTControlSAP.cpp
 * Author: rmartins
 * 
 * Created on November 25, 2010, 9:32 AM
 */

#include "FTDataSAP.h"

#include <stheno/services/stream/StreamService.h>
#include <stheno/core/p2p/p3/ft/net/data/FTDataAcceptor.h>
#include <stheno/core/p2p/p3/ft/P3ReplicationGroup.h>


FTDataSAP::FTDataSAP(P3ReplicationGroup* replicationGroup) :
m_ft(replicationGroup) {
}

FTDataSAP::~FTDataSAP() {
    close();
}

FTDataAcceptor* FTDataSAP::createAcceptor() {
    return new FTDataAcceptor(m_ft);
}
