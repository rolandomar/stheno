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
 * File:   RPCTest.cpp
 * Author: rmartins
 * 
 * Created on February 10, 2011, 6:24 PM
 */

#include "MeshTest.h"
#include <euryale/common/sleep/SleepHelper.h>

MeshTest::MeshTest() {
}

MeshTest::~MeshTest() {
}

int MeshTest::open(int argc, char** argv, Stheno* runtime) {
    printf("Open RPC test...\n");
    m_runtime = runtime;
    m_ft = false;
    m_invocations = 0;
    m_replicasNo = 0;
    static const ACE_TCHAR options[] = ACE_TEXT("::");
    ACE_Get_Opt cmd_opts(argc, argv, options);
    if (cmd_opts.long_option
            (ACE_TEXT("ft"), 'f', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("rpc"), 'r', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    //Touch of Death
    if (cmd_opts.long_option
            (ACE_TEXT("tod"), ACE_Get_Opt::NO_ARG) == -1)
        return -1;
    int option;
    m_tod = false;
    while ((option = cmd_opts()) != EOF)
        switch (option) {
            case 0:
            {
                //long_option
                const char *op = cmd_opts.long_option();
                if (ACE_OS::strcmp("tod", op) == 0) {
                    m_tod = true;
                    break;
                }
            }
            case 'r':
            {
                const char *op = cmd_opts.opt_arg();
                m_invocations = Integer::parseInt(op, 10);
                printf("%d invocations will be performed.\n", m_invocations);
                break;
            }

            case 'f':
            {
                const char *op = cmd_opts.opt_arg();
                m_ft = true;
                m_replicasNo = Integer::parseInt(op, 10);
                break;
            }
        }
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    return this->activate(reserve, cpuQoS);
}

int MeshTest::svc_i(void* arg) {
    printf("Starting RPC test...\n");
    RPCService* s = new RPCService();
    UUIDPtr sid;
    s->getSID(sid);
    UUIDPtr iid;
    ServiceParams* params = 0;
    if (m_ft) {
        FTServiceParamsPtr ftParams(new FTServiceParams(m_replicasNo, 0));
        params = new ServiceParams(sid, ftParams);
    } else {
        params = new ServiceParams(sid);
    }

    ServiceParamsPtr paramsPtr(params);
    try {
        UUIDPtr deployUUIDD;
        m_runtime->startService(sid, paramsPtr, iid, deployUUIDD);        
        ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));                
        OverlayInterface* overlay = m_runtime->getOverlayInterface();
        //overlay->getDiscovery()->executeQuery()
        
        
        return 0;
    } catch (ServiceException& se) {
        printf("MeshTest:: failed to bootstrap rpc service\n");
        return -1;
    }
    return -1;
}

void MeshTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
    RPCService s;
    UUIDPtr sid;
    s.getSID(sid);
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
        m_runtime->startService(sid, primaryUUID, paramsPtr, iid);
        ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));
        c = m_runtime->getClient(sid, iid, cp);
        rpcClient = static_cast<RPCServiceClient*> (c);
        rpcManagerObjectClient = new RPCRuntimeManagerObjectClient(rpcClient);
        rpcManagerObjectClient->hardFault();
    } catch (ServiceException& se) {
        printf("failed to inject fault\n");
    }
    if (rpcManagerObjectClient != 0) {
        delete rpcManagerObjectClient;
    }
}