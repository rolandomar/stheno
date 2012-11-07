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
 * File:   MultipleRPCTest.cpp
 * Author: rmartins
 * 
 * Created on August 5, 2011, 12:20 PM
 */

#include "MultipleRPCTest.h"
#include "ClientMultipleRPCTest.h"

#include <euryale/common/sleep/SleepHelper.h>

MultipleRPCTest::MultipleRPCTest():m_barrier(3) {
}

MultipleRPCTest::~MultipleRPCTest() {
}

int MultipleRPCTest::open(int argc, char** argv, Stheno* runtime) {
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
            (ACE_TEXT("mrpc"), 'k', ACE_Get_Opt::ARG_REQUIRED) == -1)
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
            case 'k':
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

int MultipleRPCTest::svc_i(void* arg) {
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
        //int open(int invocations,bool tod,Stheno* runtime,int prio,UUIDPtr& deployUUID, UUIDPtr& iid);
        ClientMultipleRPCTest cMaxPrio1;
        //ClientMultipleRPCTest cMaxPrio2;
        //ClientMultipleRPCTest cMaxPrio3;
        //ClientMultipleRPCTest cMaxPrio4;
        int maxRate = 25;//50;
        ClientMultipleRPCTest cMedPrio;
        int medRate = 25;//100;
        ClientMultipleRPCTest cBEPrio;
        int lowRate = 25;
        
        
        //cBEPrio.open(&m_barrier,lowRate,m_invocations,false,m_runtime,CPUQoS::LOW_RT_PRIO,deployUUIDD,iid);
        
        cMaxPrio1.open(&m_barrier,maxRate,m_invocations,m_tod,m_runtime,CPUQoS::MAX_RT_PRIO,deployUUIDD,iid);
        cMedPrio.open(&m_barrier,medRate,m_invocations,false,m_runtime,CPUQoS::MED_RT_PRIO,deployUUIDD,iid);
        //cMedPrio.open(&m_barrier,medRate,m_invocations,false,m_runtime,CPUQoS::MAX_RT_PRIO,deployUUIDD,iid);
        cBEPrio.open(&m_barrier,lowRate,m_invocations,false,m_runtime,CPUQoS::BE_PRIO,deployUUIDD,iid);
        //cMaxPrio2.open(&m_barrier,maxRate,m_invocations,m_tod,m_runtime,CPUQoS::LOW_RT_PRIO,deployUUIDD,iid);
        //cMaxPrio3.open(&m_barrier,maxRate,m_invocations,m_tod,m_runtime,CPUQoS::MAX_RT_PRIO,deployUUIDD,iid);
        //cMaxPrio4.open(&m_barrier,maxRate,m_invocations,m_tod,m_runtime,CPUQoS::MAX_RT_PRIO,deployUUIDD,iid);
        
        cMaxPrio1.wait();
        //cMaxPrio2.wait();
        //cMaxPrio3.wait();
        //cMaxPrio4.wait();
        cMedPrio.wait();
        cBEPrio.wait();
        
        //printf("ererererrerejjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj\n");
        
        ACE_Time_Value latAvg1 = cMaxPrio1.getInvocationTime();
        latAvg1 *= (double) 1 / m_invocations;
        ULongLong deltausec1;
        latAvg1.to_usec(deltausec1);
        //        
        ULongLong rebindDeltausec1;
        cMaxPrio1.getRebindTime().to_usec(rebindDeltausec1);
        
        stringstream ss1;
        ss1 << "RPCTest type(HRT) invocations(" << m_invocations << ") total_lat(" << cMaxPrio1.getInvocationTime().msec() << ") avg_lat(" << deltausec1
                << ") rebind_time(" << rebindDeltausec1 << ") missed_deadlines("<< cMaxPrio1.getMissedDeadlines()<<")" << endl;
        String l1 = ss1.str().c_str();
        LogStatisticsSingleton::instance()->logLine(l1);
        printf("Stat=%s\n",l1.c_str());
        printf("Missed deadlines=%d\n",cMaxPrio1.getMissedDeadlines());
        
        
        /*latAvg1 = cMaxPrio2.getInvocationTime();
        latAvg1 *= (double) 1 / m_invocations;        
        latAvg1.to_usec(deltausec1);        
        cMaxPrio2.getRebindTime().to_usec(rebindDeltausec1);
        
        stringstream sss2;
        sss2 << "RPCTest type(LOW) invocations(" << m_invocations << ") total_lat(" << cMaxPrio2.getInvocationTime().msec() << ") avg_lat(" << deltausec1
                << ") rebind_time(" << rebindDeltausec1 << ") missed_deadlines("<< cMaxPrio2.getMissedDeadlines()<<")" << endl;
        l1 = sss2.str().c_str();
        LogStatisticsSingleton::instance()->logLine(l1);
        printf("Stat=%s\n",l1.c_str());
        printf("Missed deadlines=%d\n",cMaxPrio2.getMissedDeadlines());
        */
        
        
        //
        ACE_Time_Value latAvg2 = cMedPrio.getInvocationTime();
        latAvg2 *= (double) 1 / m_invocations;
        ULongLong deltausec2;
        latAvg2.to_usec(deltausec2);
        //
        ULongLong rebindDeltausec2;
        cMedPrio.getRebindTime().to_usec(rebindDeltausec2);
        stringstream ss2;
        ss2 << "RPCTest type(SRT) invocations(" << m_invocations << ") total_lat(" << cMedPrio.getInvocationTime().msec() << ") avg_lat(" << deltausec2
                << ") rebind_time(" << rebindDeltausec2 << ") missed_deadlines("<< cMedPrio.getMissedDeadlines()<<")" << endl;
        String l2 = ss2.str().c_str();
        printf("Stat=%s\n",l2.c_str());
        printf("Missed deadlines=%d\n",cMedPrio.getMissedDeadlines());
        LogStatisticsSingleton::instance()->logLine(l2);
        //
        
        ACE_Time_Value latAvg3 = cBEPrio.getInvocationTime();
        latAvg3 *= (double) 1 / m_invocations;
        ULongLong deltausec3;
        latAvg3.to_usec(deltausec3);
        //
        ULongLong rebindDeltausec3;
        cBEPrio.getRebindTime().to_usec(rebindDeltausec3);
        stringstream ss3;
        ss3 << "RPCTest type(BE) invocations(" << m_invocations << ") total_lat(" << cBEPrio.getInvocationTime().msec() << ") avg_lat(" << deltausec3
                << ") rebind_time(" << rebindDeltausec3 << ") missed_deadlines("<< cBEPrio.getMissedDeadlines()<<")" << endl;
        String l3 = ss3.str().c_str();
        printf("Stat=%s\n",l3.c_str());
        printf("Missed deadlines=%d\n",cBEPrio.getMissedDeadlines());
        LogStatisticsSingleton::instance()->logLine(l3);
        //*****************************************************
        
        
        /*ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));        
        ServiceClient* c = m_runtime->getClient(sid, iid, cp);
        RPCServiceClient* rpcClient = static_cast<RPCServiceClient*> (c);
        RPCTestObjectClient* rpcTestObjectClient = new RPCTestObjectClient(rpcClient);
        Long num = 0;                
        ACE_Time_Value totalTime;
        int i = 0;
        int k = m_invocations / 2;
        int rate = 250;
        float sleepTime = ((float) 1 / (float) rate)*1000000;
        ACE_Time_Value delta;
        while (i < m_invocations) {
            if (m_tod) {
                if (i == k) {
                    performFaultInPrimary(deployUUIDD);
                    k = INT_MAX;
                }
            }
            try {
                ACE_Time_Value start = ACE_OS::gettimeofday();
                num = rpcTestObjectClient->sum(1);
                ACE_Time_Value end = ACE_OS::gettimeofday() - start;
                totalTime += end;
                if(end.msec() < sleepTime){
                    delta = ACE_Time_Value(0,(ULong)sleepTime);
                    delta -= end;
                    SleepHelper::sleep(delta);
                }               
            } catch (RPCException& ex) {
                continue;
            }
            i++;

        }
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
        LogStatisticsSingleton::instance()->logLine(l);*/
        return 0;
    } catch (ServiceException& se) {
        printf("MultipleRPCTest:: failed to bootstrap rpc service\n");
        return -1;
    }
    printf("Error on test\n");
    return -1;
}

void MultipleRPCTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
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

