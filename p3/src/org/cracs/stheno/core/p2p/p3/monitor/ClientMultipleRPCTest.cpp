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
 * Created on August 5, 2011, 11:52 AM
 */

#include "ClientMultipleRPCTest.h"
#include <euryale/common/sleep/SleepHelper.h>

ClientMultipleRPCTest::ClientMultipleRPCTest() {
}

ClientMultipleRPCTest::~ClientMultipleRPCTest() {
}

int ClientMultipleRPCTest::open(ACE_Barrier* barrier, int rate, int invocations, bool tod, Stheno* runtime, int prio, UUIDPtr& deployUUID, UUIDPtr& iid) {
    printf("Open RPC test...\n");
    m_runtime = runtime;
    m_invocations = invocations;
    m_tod = tod;
    m_prio = prio;
    m_type = 0;
    m_deployUUID = deployUUID;
    m_iid = iid;
    m_barrier = barrier;
    m_rate = rate;
    int schedule_type = 0;
    m_missedDeadlines = 0;
    switch (m_prio) {
        case CPUQoS::BE_PRIO:
        {
            m_type = QoS::BE;            
            schedule_type = CPUQoS::SCHEDULE_OTHER;
            break;
        }

        case CPUQoS::LOW_RT_PRIO:
        {
            m_type = QoS::SRT;            
            schedule_type = CPUQoS::SCHEDULE_RT_DEFAULT;
            break;

        }
        case CPUQoS::MED_RT_PRIO:
        {
            m_type = QoS::SRT;            
            schedule_type = CPUQoS::SCHEDULE_RT_DEFAULT;
            break;

        }
        case CPUQoS::MAX_RT_PRIO:
        {
            m_type = QoS::HRT;
            schedule_type = CPUQoS::SCHEDULE_RT_DEFAULT;
            break;
        }
    }


    printf("ClientRPC Scheduling %d %d\n", schedule_type, prio);
    CPUQoS* cpuQoS = new CPUPriorityQoS(schedule_type, m_prio);
    CPUReservation* reserve = 0;
    return this->activate(reserve, cpuQoS);
}

int ClientMultipleRPCTest::svc_i(void* arg) {
    printf("Starting RPC test...\n");
    RPCService* s = new RPCService();
    UUIDPtr sid;
    s->getSID(sid);
    UUIDPtr iid = m_iid;
    try {
        UUIDPtr deployUUIDD = m_deployUUID;
        //QoS::SRT, CPUQoS::MED_RT_PRIO

        //ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));
        //printf("%d %d %d %d\n",QoS::HRT, CPUQoS::MAX_RT_PRIO,m_type, m_prio);
        //printf("IID=%s\n",iid->toString().c_str());
        ClientParamsPtr cp(new ClientParams(m_type, m_prio));

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
        int rate = m_rate;
        //float sleepTime = ((float) 1 / (float) rate)*1000000;
        ULongLong sleepTime = ((double) 1 / (double) rate)*1000000;
        ACE_Time_Value delta;
        //ACE_DEBUG((LM_DEBUG,
        //  ACE_TEXT("(%t|%T)-----------INFO: before cycle\n")));
        //printf("Before cycle %d %d\n",i,m_invocations);
        m_barrier->wait();
        while (i < m_invocations) {
            //printf("cycle=%d %d\n",i,m_invocations);
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

                ULongLong usec;
                end.to_usec(usec);
                /*ACE_DEBUG((LM_DEBUG,
                        ACE_TEXT("(%t|%T)-----------INFO:Sleep Time %Q %Q\n"),
                        sleepTime, usec
                        ));*/
                //if (end.msec() < sleepTime) {
                if (usec < sleepTime) {
                    delta = ACE_Time_Value(0, (Long) sleepTime);
                    delta -= end;
                    /*ACE_DEBUG((LM_DEBUG,
                        ACE_TEXT("(%t|%T)-----------INFO:Sleep Time %d %d %Q\n"),
                            delta.msec(),sleepTime,end.msec()
                            ));*/
                    //ULongLong usec2;
                    //delta.to_usec(usec2);
                    //printf("SleepTime=%lu %lu %lu\n",usec2,sleepTime,end.msec());
                    SleepHelper::sleep(delta);
                } else {
                    m_missedDeadlines++;
                }
            } catch (RPCException& ex) {
                //sleep(0);
                //SleepHelper::sleep(0,500);
                continue;
            }
            i++;
            //printf("Invocaiton 2way = %d\n", num);
        }
        m_rebindTime = rpcClient->getRebindTotalTime();
        m_invocationTime = totalTime;

        /*ACE_Time_Value latAvg = totalTime;
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
        printf("ClientMultipleRPCTest:: failed to bootstrap rpc service\n");
        return -1;
    }
    return -1;
}

void ClientMultipleRPCTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
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

ACE_Time_Value& ClientMultipleRPCTest::getRebindTime() {
    return m_rebindTime;
}

ACE_Time_Value& ClientMultipleRPCTest::getInvocationTime() {
    return m_invocationTime;
}