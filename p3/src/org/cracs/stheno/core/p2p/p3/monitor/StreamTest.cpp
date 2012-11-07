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
 * File:   StreamTest.cpp
 * Author: rmartins
 * 
 * Created on February 11, 2011, 2:36 PM
 */

#include "StreamTest.h"
#include <euryale/common/sleep/SleepHelper.h>
#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCTestObjectClient.h>
#include <stheno/services/rpc/RPCRuntimeManagerObjectClient.h>
#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/stream/StreamClient.h>
StreamTest::StreamTest() {
}

StreamTest::~StreamTest() {
}

int StreamTest::open(int argc, char** argv, Stheno* runtime) {
    printf("Open RPC test...\n");
    m_runtime = runtime;
    m_ft = false;
    m_replicasNo = 0;
    static const ACE_TCHAR options[] = ACE_TEXT("::");
    ACE_Get_Opt cmd_opts(argc, argv, options);
    if (cmd_opts.long_option
            (ACE_TEXT("ft"), 'f', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("stream"), 's', ACE_Get_Opt::ARG_REQUIRED) == -1)
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
            case 'f':
            {
                const char *op = cmd_opts.opt_arg();
                m_ft = true;
                m_replicasNo = Integer::parseInt(op, 10);
                break;
            }
            case 's':
            {
                time_t secs = Integer::parseInt(cmd_opts.opt_arg(), 10);
                m_duration = ACE_Time_Value(secs, 0);
                //m_duration += ACE_OS::gettimeofday();
            }
            default:
            {
            }
        }
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    return this->activate(reserve, cpuQoS);
}

int StreamTest::svc_i(void* arg) {
    OverlayInterface* oi = m_runtime->getOverlayInterface();
    AudioService* s = new AudioService();
    GetSensorsQuery* query = new GetSensorsQuery();
    DiscoveryQueryReply* reply = oi->getDiscovery()->executeQuery(query);
    GetSensorsQueryReply* replyNarrow = new GetSensorsQueryReply(reply);
    //choose the correct serviceParams implementation!
    if (replyNarrow->getAudioSensorList().size() == 0) {
        printf("Monitor: Didn't find an active sensor!\n");
        delete s;
        delete query;
        delete replyNarrow;
        m_runtime->close();
        exit(-1);
    }
    SensorInfoPtr sensor = replyNarrow->getAudioSensorList().front();
    StreamQoS* qos = new StreamQoS(24, 4096);

    UUIDPtr sid;
    s->getSID(sid);
    ServiceStreamParams* serviceParams = 0;
    if (m_ft) {
        FTServiceParamsPtr ftParams(new FTServiceParams(m_replicasNo, 0));
        serviceParams =
                new ServiceStreamParams(
                sid,
                sensor->getUUID(),
                sensor->getEndpoint(),
                qos,
                ftParams);
    } else {
        serviceParams =
                new ServiceStreamParams(
                sid,
                sensor->getUUID(),
                sensor->getEndpoint(),
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
	StreamClient* streamClient = static_cast<StreamClient*> (c);
        if (m_tod) {
            printf("Performing tod\n");
            ACE_Time_Value timeToFault(m_duration);
            timeToFault *= 0.5;
            SleepHelper::sleep(timeToFault);
            performFaultInPrimary(deployUUIDD);
            SleepHelper::sleep(timeToFault);
        }else{
            printf("Non faulty run\n");
            SleepHelper::sleep(m_duration);
        }
        printf("Closing client\n");
        c->close();
	ACE_Time_Value totalEnd  = streamClient->getTotalLatency();
        ACE_Time_Value latAvg = totalEnd;
	int frames = streamClient->getCounter();
        latAvg *= (double) 1 / frames;
        ULongLong deltausec;
        latAvg.to_usec(deltausec);
        ULongLong rebindDeltausec;
        streamClient->getRebindTotalTime().to_usec(rebindDeltausec);
        ACE_DEBUG((LM_DEBUG,
                ACE_TEXT("(%t|%T)INFO: Stream total_latency(%Q)msec avg_latency(%Q)us rebind_latency(%Q)us\n"),
                totalEnd.msec(),
                deltausec,
                rebindDeltausec
                ));
	stringstream ss;
        ss << "StreamTest frames(" << frames << ") total_lat(" << totalEnd.msec() << ") avg_lat(" << deltausec 
                << ") rebind_time(" << rebindDeltausec << ")" << endl;
        String l = ss.str().c_str();
        LogStatisticsSingleton::instance()->logLine(l);
        delete c;
        return 0;
    } catch (ServiceException& se) {
        printf("failed to bootstrap audio service\n");
        return -1;
    }
    return 0;
}

void StreamTest::performFaultInPrimary(UUIDPtr& primaryUUID) {
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
