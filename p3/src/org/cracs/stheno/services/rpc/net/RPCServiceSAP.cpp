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
 * File:   RPCServiceSAP.cpp
 * Author: rmartins
 * 
 * Created on January 21, 2011, 4:24 PM
 */

#include "RPCServiceSAP.h"

#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/net/RPCServiceAcceptor.h>


RPCServiceSAP::RPCServiceSAP(RPCService* discovery) :
m_discovery(discovery) {
}

RPCServiceSAP::~RPCServiceSAP() {
    close();
}

RPCServiceAcceptor* RPCServiceSAP::createAcceptor() {
    return new RPCServiceAcceptor(m_discovery);
}
