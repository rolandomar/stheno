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
 * File:   superpeer_main.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 1, 2010, 12:29 PM
 */

#include <cstdlib>

using namespace std;
#include <stheno/Stheno.h>
#include <stheno/core/p2p/Overlay.h>

#include <ace/DLL.h>
#include <ace/svc_export.h>
#include <ace/Get_Opt.h>
/*
 * 
 */
#include <stheno/services/stream/audio/AudioService.h>
#include <stheno/services/rpc/RPCService.h>
#include <stheno/services/actuator/ActuatorService.h>
#include <stheno/core/p2p/OverlayInterface.h>

#include <euryale/qos/MedusaClient.h>

#include <ace/ARGV.h>
#include <euryale/tools/monitoring/ResourceMonitor.h>


typedef Overlay* (*create_overlay) (void);

/*int main(int argc, char** argv) {
    SthenoPtr* runtimePtr = Stheno::createRuntime();
    Stheno* runtime = runtimePtr->get();
    runtime->open();
    delete runtimePtr;
    ACE_DLL dll();
    dll.open("superpeer_plugin");
 }
 */


int
ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
    //SuperPeer p;
    ACE_UNUSED_ARG(argc);
    ACE_UNUSED_ARG(argv);
    static const ACE_TCHAR cmdline[] =
            ACE_TEXT("--prefix=superpeer");
    ACE_ARGV args1(argc, argv);
    ACE_ARGV args2(cmdline, true);
    char finalArgs[1024];
    ACE_OS::strcpy(finalArgs, args1.buf());
    ACE_OS::strcat(finalArgs, args2.buf());
    ACE_ARGV args(finalArgs, true);
    int runNO = 0;
    int testNO = 0;


    static const ACE_TCHAR options[] = ACE_TEXT("::");
    ACE_Get_Opt cmd_opts(args.argc(), args.argv(), options);
    /*if (cmd_opts.long_option
            (ACE_TEXT("config"), 'f', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;*/
    if (cmd_opts.long_option
            (ACE_TEXT("rpc"), 'r', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("stream"), 's', ACE_Get_Opt::NO_ARG) == -1)
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

    int option;
    while ((option = cmd_opts()) != EOF)
        switch (option) {
            case 0:
            {
                const char *op = cmd_opts.long_option();
                printf("LONG OPTI\n");
                /*if (ACE_OS::strcmp("faults", op) == 0) {
                    faults = true;
                    faultArgs = cmd_opts.opt_arg();
                    printf("Faults=%s\n", faultArgs);
                    //FaultManager fm;
                    //fm.attachFaults(faultArgs);
                    break;
                }*/

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

            case ':':
            {
                ACE_ERROR_RETURN
                        ((LM_ERROR, ACE_TEXT("-%c requires an argument\n"),
                        cmd_opts.opt_opt()), -1);
                exit(-1);
            }
            default:
            {
            }
        }


    ACE_DLL dll;
    //int retval = dll.open(ACE_DLL_PREFIX ACE_TEXT("(%T)superpeer_plugin"));
    int retval = dll.open("libsuperpeer_plugin");

    if (retval != 0) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) Error openning plugin!\n")));
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
        printf("Starting superpeer...(%d)\n", gettid());
        /*GlobalMedusaClient::instance()->open();
        list<CPUPartition*>* partitions = GlobalMedusaClient::instance()->getCPUPartitionList();
        if(partitions == 0){
            printf("No cpu partitions?\n");
            exit(-1);
        }
        list<CPUPartition*>::iterator iter = partitions->begin();
        while(iter != partitions->end()){
            printf("partitions=%s\n",(*iter)->getName().c_str());
            iter++;
        }*/

        Stheno* runtime = 0;
        try {
            Overlay* overlay = mc();
            runtime = Stheno::createRuntime();
            printf("created runtime(%p)...\n", overlay);
            runtime->attachOverlay(overlay);
            printf("Attached the overlay (%p)...\n", overlay);
        } catch (RuntimeException& ex) {
            printf("Runtime exception=%s\n", ex.message().c_str());
            exit(-1);
            //Backtrace::backstrace();
        }

        /*String runtimeGroupPath = "runtime_"+runtime->getUUID()->toString();
        
        String cpusetDomain("HRT/");
        cpusetDomain+=runtimeGroupPath;
        
        String scheduleDomain(runtimeGroupPath);
        //test
        String scheduleServiceDomain(runtimeGroupPath+"/service_"+runtime->getUUID()->toString());        
        //scheduleServiceDomain += "/service_"+runtimeUUID;
        CPUGroupQoS serviceQos(cpusetDomain,scheduleServiceDomain,10000,300000,1024);
        //endtest
        
        //QoS for root of runtime's reservation group
        CPUGroupQoS qos(cpusetDomain,scheduleDomain,20000,300000,1024);
        //create the reservation group for this runtime
        GlobalMedusaClient::instance()->createGroupCPUReserve(scheduleDomain,qos);        
        
        GlobalMedusaClient::instance()->createGroupCPUReserve(scheduleServiceDomain,serviceQos);        
        
        CPUPriorityRTFifo* qosTask = new CPUPriorityRTFifo(50);
        GlobalMedusaClient::instance()->setTaskInCPUPartition(gettid(),cpusetDomain,scheduleDomain,qosTask);
         */
        try {
            //populate service 
            AudioService* audioService = new AudioService(runtime->duplicate());
            runtime->getServiceFactory().addService(audioService);
            RPCService* rpcService = new RPCService(runtime->duplicate());
            runtime->getServiceFactory().addService(rpcService);
            ActuatorService* actuatorService = new ActuatorService(runtime->duplicate());
            runtime->getServiceFactory().addService(actuatorService);
            runtime->open(args);
        } catch (RuntimeException& ex) {
            printf("Runtime exception2=%s\n", ex.message().c_str());
        }
        stringstream resourceFile;
        UUIDPtr runtimeUUID;
        runtime->getUUID(runtimeUUID);
        resourceFile << "resource_superpeer_" << testNO << "_" << runNO << "_" << runtimeUUID->toString().c_str() << ".log";
        ResourceMonitor rm;
        rm.open(resourceFile.str().c_str());
        //while (getchar() != 'q');
        while (true) sleep(1000000);
        delete runtime;

        //magazine->getUUID();
    }

    dll.close();
    getchar();
    return 0;
}
