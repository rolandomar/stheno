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
 * File:   MultipleRPCTest.h
 * Author: rmartins
 *
 * Created on August 5, 2011, 12:20 PM
 */

#ifndef MULTIPLERPCTEST_H
#define	MULTIPLERPCTEST_H

#include <cstdlib>
#include <stheno/Stheno.h>
#include <stheno/core/p2p/Overlay.h>
using namespace std;
#include <ace/DLL.h>
#include <ace/svc_export.h>
#include <ace/Get_Opt.h>

#include <euryale/common/Integer.h>

#include <stheno/core/p2p/OverlayInterface.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQuery.h>
#include <stheno/core/p2p/discovery/general_provider/GetSensorsQueryReply.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQuery.h>
#include <stheno/core/p2p/discovery/general_provider/LaunchServiceSiteQueryReply.h>

//service
#include <stheno/services/stream/audio/AudioService.h>
#include <stheno/services/stream/ServiceStreamParams.h>

#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/rpc/RPCTestObjectClient.h>
#include <stheno/services/rpc/RPCRuntimeManagerObjectClient.h>

#include <stheno/services/actuator/ActuatorService.h>
#include <stheno/services/actuator/ActuatorServiceClient.h>



#include <ace/Sched_Params.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

#include "stheno/core/p2p/p3/common/faults/FaultManager.h"

#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQuery.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3FindPeerQueryReply.h>

#include <euryale/tools/monitoring/ResourceMonitor.h>
#include <stheno/common/LogStatistics.h>

#include <ace/Barrier.h>

class MultipleRPCTest : public Task {
public:

    MultipleRPCTest();

    virtual ~MultipleRPCTest();

    int open(int argc, char** argv, Stheno* runtime);

    virtual int svc_i(void* arg);

protected:
    bool m_ft;
    int m_replicasNo;
    int m_invocations;
    Stheno* m_runtime;
    bool m_tod;
    ACE_Barrier m_barrier;

    void performFaultInPrimary(UUIDPtr& primaryUUID);
    
    virtual int close (u_long flags = 0){return 0;}
};


#endif	/* MULTIPLERPCTEST_H */

