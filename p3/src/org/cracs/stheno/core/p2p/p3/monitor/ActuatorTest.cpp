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
 * File:   ActuatorTest.cpp
 * Author: rmartins
 * 
 * Created on February 10, 2011, 6:33 PM
 */

#include "ActuatorTest.h"
#include <euryale/common/sleep/SleepHelper.h>
#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCTestObjectClient.h>
#include <stheno/services/rpc/RPCRuntimeManagerObjectClient.h>

ActuatorTest::ActuatorTest() {
}

ActuatorTest::~ActuatorTest() {
}

int ActuatorTest::open(int argc, char** argv, Stheno* runtime) {
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
            (ACE_TEXT("rpc"), 'a', ACE_Get_Opt::ARG_REQUIRED) == -1)
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
                break;
            }
            case 'a':
            {
                const char *op = cmd_opts.opt_arg();
                m_invocations = Integer::parseInt(op, 10);
                printf("%d actions will be performed.\n", m_invocations);
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

int ActuatorTest::svc_i(void* arg) {
    OverlayInterface* oi = m_runtime->getOverlayInterface();
    ActuatorService* s = new ActuatorService();
    GetSensorsQuery* query = new GetSensorsQuery();
    DiscoveryQueryReply* reply = oi->getDiscovery()->executeQuery(query);
    GetSensorsQueryReply* replyNarrow = new GetSensorsQueryReply(reply);
    //choose the correct serviceParams implementation!
    if (replyNarrow->getActuatorSensorList().size() == 0) {
        printf("Monitor: Didn't find an active sensor!\n");
        delete s;
        delete query;
        delete replyNarrow;
        m_runtime->close();
        exit(-1);
    }
    //SensorInfoPtr sensor = replyNarrow->getActuatorSensorList().front();
    ActuatorQoS* qos = new ActuatorQoS(24, 4096);

    UUIDPtr sid;
    s->getSID(sid);
    ServiceActuatorParams* serviceParams = 0;
    if (m_ft) {
        FTServiceParamsPtr ftParams(new FTServiceParams(m_replicasNo, 0));
        serviceParams =
                new ServiceActuatorParams(
                sid,
                replyNarrow->getActuatorSensorList(),
                qos,
                ftParams);
    } else {
        serviceParams =
                new ServiceActuatorParams(
                sid,
                replyNarrow->getActuatorSensorList(),
                qos);
    }
    //ServiceStreamParams* serviceParams = 0;
    ServiceParamsPtr serviceParamsPtr(serviceParams);
    UUIDPtr iid;

    try {
        UUIDPtr deployUUIDD;
        m_runtime->startService(sid, serviceParamsPtr, iid, deployUUIDD);
        ClientParamsPtr cp(new ClientParams(QoS::HRT, CPUQoS::MAX_RT_PRIO));
        ServiceClient* c = m_runtime->getClient(sid, iid, cp);
        ActuatorServiceClient* actuatorClient = static_cast<ActuatorServiceClient*> (c);
        String msg("Painel Message\n\0");
        ByteOutputStream os(1024);
        os.write_string(msg);
        //float sleepTime = ((float) 1 / (float) 50)*1000000;
        ACE_Time_Value start = ACE_OS::gettimeofday();
        ACE_Time_Value total;
        int k = m_invocations / 2;
        int i = 0;
        int rate = 250;
        float sleepTime = ((float) 1 / (float) rate)*1000000;
        ACE_Time_Value delta;
        //for (int i = 0; i < m_invocations; i++) {
        while (i < m_invocations) {
            if (m_tod) {
                if (i == k) {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Actuador sending tod...\n")));
                    performFaultInPrimary(deployUUIDD);
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Actuador after tod...\n")));
                    k = INT_MAX;
                }
            }
            try {
                ACE_Time_Value startAction = ACE_OS::gettimeofday();
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Actuador sending action...\n")));
                actuatorClient->action(os.current_->duplicate());
                
                ACE_Time_Value end = ACE_OS::gettimeofday() -startAction;
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Actuador after action. time(%Q)\n"),end.msec()));
                //total += (endAction - startAction);
                total += end;
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
            } catch (ActuatorException& ex) {
                continue;
            } catch (ServiceException& servcieEx) {
                continue;
            }

            i++;
            /*ULongLong deltausec;
            (endAction-startAction).to_usec(deltausec);
            ACE_DEBUG((LM_DEBUG,
                  ACE_TEXT("(%t|%T)INFO: Actuador action(%Q)us\n"),                
                  deltausec
                  ));*/
            //SleepHelper::sleep(0, sleepTime);
        }
        //ACE_Time_Value end  = ACE_OS::gettimeofday() - start;
        ACE_Time_Value totalEnd = total; //ACE_OS::gettimeofday() - start;
        ACE_Time_Value latAvg = totalEnd;
        latAvg *= (double) 1 / m_invocations;
        ULongLong deltausec;
        latAvg.to_usec(deltausec);
        
        ULongLong rebindDeltausec;
        actuatorClient->getRebindTotalTime().to_usec(rebindDeltausec);
        
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: Actuador total_latency(%Q)msec avg_latency(%Q)us rebind_latency(%Q)us\n"),
                totalEnd.msec(),
                deltausec,
                rebindDeltausec
                ));
        stringstream ss;
        ss << "ActuatorTest invocations(" << m_invocations << ") total_lat(" << totalEnd.msec() << ") avg_lat(" << deltausec << ")" 
                << " rebind_time(" << rebindDeltausec << ")" << endl;
        String l = ss.str().c_str();
        LogStatisticsSingleton::instance()->logLine(l);
        return 0;
    } catch (ServiceException& se) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: failed to bootstrap actuator service\n")));
        return -1;
    }
    return -1;

}

void ActuatorTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
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
