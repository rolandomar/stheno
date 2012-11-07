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
 * File:   load.cpp
 * Author: rmartins
 *
 * Created on February 11, 2011, 12:16 PM
 */

#include <cstdlib>

using namespace std;
#include <ace/DLL.h>
#include <ace/svc_export.h>
#include <ace/Get_Opt.h>


#include <euryale/tools/loader/Loader.h>
#include <euryale/common/Integer.h>

#include "Loader.h"
#include <euryale/tools/monitoring/ResourceMonitor.h>

int main(int argc, char** argv) {
    bool verbose = false;
    CPUQoS* qos = 0;
    int prio = 0;
    int timeout = -1;
    int threads = ACE_OS::num_processors();
    int load = 100;
    bool fifo = false;
    bool rr = false;
    static const ACE_TCHAR options[] = ACE_TEXT("::v");
    ACE_Get_Opt cmd_opts(argc, argv, options);
    if (cmd_opts.long_option
            (ACE_TEXT("rt"), ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("threads"), 't', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    if (cmd_opts.long_option
            (ACE_TEXT("prio"), 'p', ACE_Get_Opt::ARG_REQUIRED) == -1)
        return -1;
    int option;
    while ((option = cmd_opts()) != EOF)
        switch (option) {
            case 'v':
            {
                verbose = true;
                printf("Verbose mode is on.\n");
                break;
            }
            case 0:
            {
                //long_option
                const char *op = cmd_opts.long_option();
                if (ACE_OS::strcmp("rt", op) == 0) {
                    const char *op = cmd_opts.opt_arg();
                    if (ACE_OS::strcmp(op, "fifo") == 0) {
                        fifo = true;
                    } else {
                        if (ACE_OS::strcmp(op, "rr") == 0) {
                            rr = true;
                        } else {
                            return -1;
                        }
                    }
                    break;
                }
                break;
            }

            case 'p':
            {
                const char *op = cmd_opts.opt_arg();
                prio = Integer::parseInt(op, 10);
                if(prio > CPUQoS::MAX_RT_PRIO) {
                    prio = CPUQoS::MAX_RT_PRIO;
                }
                break;
            }

            case 't':
            {
                const char *op = cmd_opts.opt_arg();
                threads = Integer::parseInt(op, 10);
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
                const char *op = cmd_opts.opt_arg();
                ACE_ERROR_RETURN
                        ((LM_ERROR, ACE_TEXT("Parse error %s.\n"), op), -1);
                exit(-1);
            }
        }

    if (fifo) {
        if (prio == 0) {
            prio = 50;
        }
        qos = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, prio);
    } else {
        if (rr) {
            if (prio == 0) {
                prio = 50;
            }
            qos = new CPUPriorityQoS(CPUQoS::SCHEDULE_RR, prio);
        }
    }
    int optind = cmd_opts.opt_ind();    
    if (optind >= argc) {
        printf("Usage: load --rt {fifo,rr} -t thread_no -p prio load_per [duration(s)]\n");
        return -1;
    }
    load = Integer::parseInt(argv[optind++], 10);
    if (optind < argc) {
        timeout = Integer::parseInt(argv[optind++], 10);
        printf("timeout=%d\n", timeout);
    }
    /*if (argc > optind) {
        printf("Usage: load -rt {fifo,rr} -t thread_no -p prio load_per [duration(s)]\n");
        return -1;
    }*/
    if (verbose) {
        if (rr) {
            printf("Load with SCHED_RR prio=%d\n",prio);
        } else {
            if(fifo){
                printf("Load with SCHED_FIFO prio=%d\n",prio);   
            }else{
                printf("Load with SCHED_OTHER\n");
            }
        }
    }

    ACE_Time_Value* deadline = 0;
    if(timeout != -1){
        deadline = new ACE_Time_Value(timeout, 0);
    }
    /*ResourceMonitor rm;
    rm.open(0);*/
    
    Loader loader;
    loader.open(load, threads, deadline, 0, qos,verbose);
    loader.close(true);
    return 0;
}

