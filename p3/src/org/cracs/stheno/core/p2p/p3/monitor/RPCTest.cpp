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

#include "RPCTest.h"
#include <euryale/common/sleep/SleepHelper.h>

RPCTest::RPCTest() {
}

RPCTest::~RPCTest() {
}

int RPCTest::open(int argc, char** argv, Stheno* runtime) {
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

int RPCTest::svc_i(void* arg) {
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


    //NO-FT
    //ServiceParams* params = new ServiceParams(sid);            
    ServiceParamsPtr paramsPtr(params);
    try {
        UUIDPtr deployUUIDD;
        m_runtime->startService(sid, paramsPtr, iid, deployUUIDD);
        ACE_DEBUG((LM_DEBUG,
                        ACE_TEXT("(%t|%T)-----------INFO: after serice started\n")));
        if(m_invocations==0){
            ACE_DEBUG((LM_DEBUG,
                        ACE_TEXT("(%t|%T) INFO:RPCTest:svc() - no invocations.")));
            return 0;
        }
        
        ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));
        //ClientParamsPtr cp(new ClientParams(QoS::BE,0));
        ServiceClient* c = m_runtime->getClient(sid, iid, cp);
        RPCServiceClient* rpcClient = static_cast<RPCServiceClient*> (c);
        /*RPCRuntimeManagerObjectClient* rpcManagerObjectClient = new RPCRuntimeManagerObjectClient(rpcClient);
        rpcManagerObjectClient->hardFault();*/
        RPCTestObjectClient* rpcTestObjectClient = new RPCTestObjectClient(rpcClient);
        //rpcTestObjectClient->ping();
        Long num = 0;        
        //for (int i = 0; i < m_invocations; i++) {
        ACE_Time_Value totalTime;
        int i = 0;
        int k = m_invocations / 2;
        int rate = 250;
        float sleepTime = ((float) 1 / (float) rate)*1000000;
        ACE_Time_Value delta;
        //ACE_DEBUG((LM_DEBUG,
                      //  ACE_TEXT("(%t|%T)-----------INFO: before cycle\n")));
        while (i < m_invocations) {
            if (m_tod) {
                if (i == k) {
                    //ACE_DEBUG((LM_DEBUG,
                        //ACE_TEXT("(%t|%T)-----------INFO: Kill primary\n")));
                    performFaultInPrimary(deployUUIDD);
                    //ACE_DEBUG((LM_DEBUG,
                      //  ACE_TEXT("(%t|%T)-----------INFO: fater kill primary\n")));
                    k = INT_MAX;
                }
            }
            try {
                ACE_Time_Value start = ACE_OS::gettimeofday();
                //ACE_DEBUG((LM_DEBUG,
                  //      ACE_TEXT("(%t|%T)-----------INFO: Inovcating\n")));
                num = rpcTestObjectClient->sum(1);
                //ACE_DEBUG((LM_DEBUG,
                  //      ACE_TEXT("(%t|%T)-----------INFO: Inovcated\n")));
                ACE_Time_Value end = ACE_OS::gettimeofday() - start;
                totalTime += end;
                if(end.msec() < sleepTime){
                    delta = ACE_Time_Value(0,(ULong)sleepTime);
                    delta -= end;
                    /*ACE_DEBUG((LM_DEBUG,
                        ACE_TEXT("(%t|%T)-----------INFO:Sleep Time %d %d %Q\n"),
                            delta.msec(),sleepTime,end.msec()
                            ));*/
                    //printf("SleepTime=%d %d %lu\n",delta.msec(),sleepTime,end.msec());
                    SleepHelper::sleep(delta);
                }               
            } catch (RPCException& ex) {
                //sleep(0);
                //SleepHelper::sleep(0,500);
                continue;
            }
            i++;
            //printf("Invocaiton 2way = %d\n", num);
        }
        //printf("Last Invocation 2way result = %d\n", num);
        //ACE_Time_Value end = ACE_OS::gettimeofday();
        //end -= start;
        ACE_Time_Value latAvg = totalTime;
        latAvg *= (double) 1 / m_invocations;
        ULongLong deltausec;
        latAvg.to_usec(deltausec);


        ULongLong rebindDeltausec;
        rpcClient->getRebindTotalTime().to_usec(rebindDeltausec);

        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: rpc total_latency(%Q)msec avg_latency(%Q)us rebind_latency(%Q)us\n"),
                totalTime.msec(),
                deltausec,
                rebindDeltausec
                ));


        stringstream ss;
        ss << "RPCTest invocations(" << m_invocations << ") total_lat(" << totalTime.msec() << ") avg_lat(" << deltausec
                << ") rebind_time(" << rebindDeltausec << ")" << endl;
        String l = ss.str().c_str();
        LogStatisticsSingleton::instance()->logLine(l);
        return 0;
    } catch (ServiceException& se) {
        printf("RPCTest:: failed to bootstrap rpc service\n");
        return -1;
    }
    return -1;
}

void RPCTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
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