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
 * File:   FaultManager.cpp
 * Author: rmartins
 * 
 * Created on February 8, 2011, 12:44 PM
 */

#include "FaultManager.h"
#include <euryale/common/sleep/SleepHelper.h>
#include <ace/Tokenizer_T.h>
#include <stheno/Stheno.h>
#include <stheno/core/p2p/p3/common/faults/Fault.h>
#include <stheno/core/p2p/p3/common/faults/FaultFactory.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQuery.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQueryReply.h>
#include <stheno/core/p2p/OverlayInterface.h>

#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCTestObjectClient.h>
#include <stheno/services/rpc/RPCRuntimeManagerObjectClient.h>

FaultManager::FaultManager(Stheno* runtime) :
m_runtime(runtime) {

}

FaultManager::~FaultManager() {
}

int FaultManager::open(int numCrashFaults, int numTempFaults, int delay) {
    SleepHelper::sleep(delay);
    generateFaults(numCrashFaults, numTempFaults);
    return this->activate();
}

bool FaultManager::attachFaults(const char* faults) {
    return parseFaults(faults);
}

bool FaultManager::parseFaults(const char* faults) {
    //m_faults = faults;
    //printf("Parsing\n");
    ACE_OS::memset(m_faults, 0, MAX_BUFFER_SIZE);
    ACE_OS::strcpy(m_faults, faults);
    ACE_Tokenizer_T<char> tokenizer(m_faults);
    //tokenizer.delimiter(':');
    tokenizer.delimiter_replace(':', 0);
    for (char *p = tokenizer.next(); p; p = tokenizer.next()) {
        if (!parseFault(p)) {
            return false;
        }
    }
    return true;
}

bool FaultManager::parseFault(char* faultStr) {
    //printf("Fault=%s\n",faultStr);
    Fault* fault = FaultFactory::createFault(faultStr);
    if (fault == 0) {
        return false;
    }
    printf("Fault=%s<--\n", fault->toString().c_str());
    m_faultList.push_back(fault);
    //const char* fault = tokenizer.next ();
    return true;
}

void FaultManager::generateFaults(int crashFaults, int tempFaults) {
    ACE_Time_Value now = ACE_OS::gettimeofday();
    for (int i = 0; i < crashFaults; i++) {
        CrashFault* cf = new CrashFault(INT_MAX, now);
        m_faultList.push_back(cf);
    }
    for (int i = 0; i < tempFaults; i++) {
        TempFault* tf = new TempFault(INT_MAX, now, now);
        m_faultList.push_back(tf);
    }
}

int FaultManager::svc_i(void* arg) {
    while (m_faultList.size() > 0) {
        Fault* f = m_faultList.front();
        if (f->isOverdue()) {
            performFault(f);
            m_faultList.pop_front();
        } else {
            f->sleepUntilDeadline();
        }
    }
    return 0;
}

void FaultManager::performFault(Fault* f) {
    OverlayInterface* oi = m_runtime->getOverlayInterface();
    P3FindPeerQuery* queryPeer = new P3FindPeerQuery(INT_MAX);
    DiscoveryQueryReply* replyPeer = oi->getDiscovery()->executeQuery(queryPeer);
    P3FindPeerQueryReply* replyPeerNarrow = new P3FindPeerQueryReply(replyPeer);
    if (replyPeerNarrow->isException()) {
        printf("Didnt find random SP!\n");
    } else {
        printf("Found random SP=%s\n", replyPeerNarrow->getPeerInfo()->getPID()->toString().c_str());
    }
    RPCService s;
    UUIDPtr sid;
    s.getSID(sid);
    //UUIDPtr sid(s.getSID());
    UUIDPtr iid;
    ServiceParams* params = 0;
    params = new ServiceParams(sid);
    //NO-FT
    //ServiceParams* params = new ServiceParams(sid);            
    ServiceParamsPtr paramsPtr(params);
    ServiceClient* c = 0;
    RPCServiceClient* rpcClient = 0;
    RPCRuntimeManagerObjectClient* rpcManagerObjectClient = 0;
    try {
        UUIDPtr deployUUIDD;
        m_runtime->startService(sid, paramsPtr, iid,deployUUIDD);
        ClientParamsPtr cp(new ClientParams(QoS::HRT,CPUQoS::MAX_RT_PRIO));
        c = m_runtime->getClient(sid, iid,cp);
        rpcClient = static_cast<RPCServiceClient*> (c);
        rpcManagerObjectClient = new RPCRuntimeManagerObjectClient(rpcClient);
        rpcManagerObjectClient->hardFault();
    } catch (ServiceException& se) {
        printf("failed to inject fault\n");
    }
    if (rpcManagerObjectClient != 0) {
        delete rpcManagerObjectClient;
    }
    delete f;
}