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
 * File:   leaf_main.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 2, 2010, 11:07 AM
 */

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

#include <stheno/core/p2p/p3/monitor/RPCTest.h>
#include <stheno/core/p2p/p3/monitor/MultipleRPCTest.h>
#include <stheno/core/p2p/p3/monitor/ActuatorTest.h>
#include <stheno/core/p2p/p3/monitor/StreamTest.h>

#include <euryale/tools/loader/Loader.h>
#include <ace/ARGV.h>
/*
 * 
 */

typedef Overlay* (*create_overlay) (void);


void performFault(Stheno* runtime,int lvl){
    int level = INT_MAX;
    if( lvl >= 0){
        level = lvl;
    }
    printf("Performing Fault(%d)...\n",level);
    OverlayInterface* oi = runtime->getOverlayInterface();
    
    P3FindPeerQuery* queryPeer = new P3FindPeerQuery(level);
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
    UUIDPtr iid;    
    ServiceParams* params = 0;
    params = new ServiceParams(sid);
    ServiceParamsPtr paramsPtr(params);
    ServiceClient* c = 0;
    RPCServiceClient* rpcClient = 0;
    RPCRuntimeManagerObjectClient* rpcManagerObjectClient = 0;
    try {
        UUIDPtr deployUUIDD;
        deployUUIDD = replyPeerNarrow->getPeerInfo()->getPID();
        //runtime->startService(sid, paramsPtr, iid,deployUUIDD);
        runtime->startService(sid, deployUUIDD,paramsPtr, iid);
        ClientParamsPtr cp(new ClientParams(QoS::HRT,CPUQoS::MAX_RT_PRIO));
        c = runtime->getClient(sid, iid,cp);
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

int main(int argc, char** argv) {
    /*ACE_Time_Value* timeout = new ACE_Time_Value(20,0);
    Loader load;
    load.open(75,16,timeout);
    load.close(true);
    printf("after loader\n");*/
    //SuperPeer p;
    ACE_UNUSED_ARG(argc);
    ACE_UNUSED_ARG(argv);
    static const ACE_TCHAR cmdline[] =
            ACE_TEXT("--prefix=monitor");
    ACE_ARGV args1(argc, argv);
    ACE_ARGV args2(cmdline, true);
    char finalArgs[1024];
    ACE_OS::strcpy(finalArgs, args1.buf());
    ACE_OS::strcat(finalArgs, args2.buf());
    ACE_ARGV args(finalArgs, true);

    for (int i = 0; i < args.argc(); i++) {
        printf("%s\n", args[i]);
    }

    bool startAudio = false;
    bool startRPC = false;
    bool startMultipleRPC = false;
    bool startActuator = false;
    bool ft = false;
    int replicasNo = 0;
    int invocations = 0;
    bool faults = false;
    const char *faultArgs = 0;
    RPCTest rpcTest;
    MultipleRPCTest mrpcTest;
    ActuatorTest actuatorTest;
    StreamTest streamTest;
    int runNO = 0;
    int testNO = 0;
    //new
    bool injectFault = false;
    int faultLevel = -1;


    static const ACE_TCHAR options[] = ACE_TEXT("::");
    ACE_Get_Opt cmd_opts(args.argc(), args.argv(), options);
    /*if (cmd_opts.long_option
            (ACE_TEXT("config"), 'f', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;*/
    if (cmd_opts.long_option
            (ACE_TEXT("rpc"), 'r', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("mrpc"), 'k', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("stream"), 's', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;

    if (cmd_opts.long_option
            (ACE_TEXT("ft"), 'f', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("actuator"), 'a', ACE_Get_Opt::NO_ARG) == -1)
        return -1;

    if (cmd_opts.long_option
            (ACE_TEXT("faults"), ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("run"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        return -1;
    }

    if (cmd_opts.long_option
            (ACE_TEXT("test"), ACE_Get_Opt::ARG_REQUIRED) == -1) {
        return -1;
    }
    
    if (cmd_opts.long_option
            (ACE_TEXT("inject-fault"), ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;

    int option;
    //ACE_TCHAR config_file[MAXPATHLEN];
    //ACE_OS_String::strcpy(config_file, ACE_TEXT("HAStatus.conf"));
    while ((option = cmd_opts()) != EOF)
        switch (option) {
                /*case 'f':
                {
                    ACE_OS_String::strncpy(config_file,
                            cmd_opts.opt_arg(),
                            MAXPATHLEN);
                    break;
                }*/
            case 0:
            {
                //long_option
                const char *op = cmd_opts.long_option();
                printf("LONG OPTI\n");
                if (ACE_OS::strcmp("inject-fault", op) == 0) {
                    
                    const char *op = cmd_opts.opt_arg();
                    injectFault = true;
                    faultLevel = Integer::parseInt(op, 10);
                    printf("Injecting fault at level=%d\n",faultLevel);
                    break;
                }
                if (ACE_OS::strcmp("faults", op) == 0) {
                    faults = true;
                    faultArgs = cmd_opts.opt_arg();
                    printf("Faults=%s\n", faultArgs);
                    /*FaultManager fm;
                    fm.attachFaults(faultArgs);*/
                    break;
                }
                if (ACE_OS::strcmp("rpc", op) == 0) {
                    {
                        startRPC = true;
                        const char *op = cmd_opts.opt_arg();
                        invocations = Integer::parseInt(op, 10);
                        printf("%d invocations will be performed.\n", invocations);
                    }
                    break;
                }
                if (ACE_OS::strcmp("mrpc", op) == 0) {
                    {
                        startMultipleRPC = true;
                        const char *op = cmd_opts.opt_arg();
                        invocations = Integer::parseInt(op, 10);
                        printf("MultipleRPC with %d invocations will be performed.\n", invocations);
                    }
                    break;
                }
                if (ACE_OS::strcmp("run", op) == 0) {
                    runNO = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("RunNo=%d %s\n", runNO, cmd_opts.opt_arg());
                    break;
                }

                if (ACE_OS::strcmp("test", op) == 0) {
                    testNO = Integer::parseInt(cmd_opts.opt_arg(), 10);
                    printf("TestNo=%d %s\n", testNO, cmd_opts.opt_arg());
                    break;
                }
            }
            case 'f':
            {
                const char *op = cmd_opts.opt_arg();
                ft = true;
                replicasNo = Integer::parseInt(op, 10);
                break;
            }
            case 'r':
            {
                startRPC = true;
                const char *op = cmd_opts.opt_arg();
                invocations = Integer::parseInt(op, 10);
                printf("%d invocations will be performed.\n", invocations);

                break;
            }

            case 'k':
            {
                startMultipleRPC = true;
                const char *op = cmd_opts.opt_arg();
                invocations = Integer::parseInt(op, 10);
                printf("MultipleRPC with %d invocations will be performed.\n", invocations);

                break;
            }
            case 's':
            {
                startAudio = true;
                printf("Stream service is going to be activated.\n");
                break;
            }

            case 'a':
            {
                startActuator = true;
                printf("Actuator service is going to be activated.\n");
                break;
            }

            case ':':
            {
                ACE_ERROR_RETURN
                        ((LM_ERROR, ACE_TEXT("-%c requires an argument\n"),
                        cmd_opts.opt_opt()), -1);
                exit(-1);
            }
            default:
            {
                /*ACE_ERROR_RETURN
                        ((LM_ERROR, ACE_TEXT("Parse error!\n")), -1);
                exit(-1);*/
            }
        }


    ACE_DLL dll;
    int retval = dll.open("libmonitor_plugin");

    if (retval != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)HEERE: error\n")));
        ACE_ERROR_RETURN((LM_ERROR,
                "%@",
                "dll.open"),
                -1);
    }
    create_overlay mc = 0;

    // Cast the void* to non-pointer type first - it's not legal to
    // cast a pointer-to-object directly to a pointer-to-function.
    void *void_ptr = dll.symbol(ACE_TEXT("create_overlay"));

    ptrdiff_t tmp = reinterpret_cast<ptrdiff_t> (void_ptr);

    mc = reinterpret_cast<create_overlay> (tmp);

    if (mc == 0) {
        ACE_ERROR_RETURN((LM_ERROR,
                "%@",
                "dll.symbol"),
                -1);
    }

    {
        Overlay* overlay = mc();
        Stheno* runtime = Stheno::createRuntime();
        runtime->attachOverlay(overlay);
        try {
            runtime->open(args);
            AudioService* audioService = new AudioService(runtime->duplicate());
            runtime->getServiceFactory().addService(audioService);
            RPCService* rpcService = new RPCService(runtime->duplicate());
            runtime->getServiceFactory().addService(rpcService);
            ActuatorService* actuatorService = new ActuatorService(runtime->duplicate());
            runtime->getServiceFactory().addService(actuatorService);
        } catch (RuntimeException& ex) {
            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t|%T) Failed to bootstrap runtime!\n")));
            delete runtime;
            exit(-1);
        }

        //OverlayInterface* oi = runtime->getOverlayInterface();

        /*ACE_Sched_Params s(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
        if (ACE_OS::sched_params(s, gettid() != 0)) {
            exit(-1);
        }*/

        /*P3FindPeerQuery* queryPeer = new P3FindPeerQuery(INT_MAX);
        DiscoveryQueryReply* replyPeer = oi->getDiscovery()->executeQuery(queryPeer);
        P3FindPeerQueryReply* replyPeerNarrow = new P3FindPeerQueryReply(replyPeer);
        if(replyPeerNarrow->isException()){
            printf("Didnt find random SP!\n");
        }else{
            printf("Found random SP=%s\n",replyPeerNarrow->getPeerInfo()->getPID()->toString().c_str());
        }*/
        /*if(injectFault){
            performFault(runtime,faultLevel);
        }*/
        
        
        stringstream resourceFile;
        UUIDPtr runtimeUUID;
        runtime->getUUID(runtimeUUID);
        resourceFile << "resource_monitor_" << testNO << "_" << runNO << "_" << runtimeUUID->toString().c_str() << ".log";
        ResourceMonitor rm;
        rm.open(resourceFile.str().c_str());
    

        if (startAudio) {
            streamTest.open(argc, argv, runtime);
            streamTest.wait();
        }

        if (startRPC) {
            rpcTest.open(argc, argv, runtime);
            rpcTest.wait();
        }
        
        if (startMultipleRPC) {
            mrpcTest.open(argc, argv, runtime);
            mrpcTest.wait();
        }

        if (startActuator) {
            actuatorTest.open(argc, argv, runtime);
            actuatorTest.wait();
        }
        
        if(injectFault){
            performFault(runtime,faultLevel);
        }

        //FaultManager fm(runtime);
        //fm.open(1,0);

        //while (getchar() != 'q');
        //while (true) sleep(1000000);

        printf("DELETE Runtime!!!!\n");
        delete runtime;
    }

    dll.close();
    //getchar();
    return 0;
}


