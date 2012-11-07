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
 * File:   hades_test.cpp
 * Author: rmartins
 *
 * Created on December 19, 2009, 10:32 AM
 */

#include <stdlib.h>

/*
 * 
 */
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_sys_time.h"
#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>
//#include <euryale/qos/hadesd/net/HadesdAcceptor.h>
#include <euryale/qos/net/HadesClientHandler.h>
//#include <euryale/qos/hadesd/net/packet/ThreadReserveRequestPacket.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>


int main(int argc, char** argv) {

    //ACE_INET_Addr port_to_connect (ACE_TEXT ("HAStatus"), ACE_LOCALHOST);
    //CE_INET_Addr port_to_connect(55555, ACE_LOCALHOST);
    ACE_UNIX_Addr addr("/var/hadesd");
    ACE_Connector<HadesClientHandler, ACE_LSOCK_CONNECTOR> connector;
    HadesClientHandler client;
    HadesClientHandler *pc = &client;
    if (connector.connect(pc, addr) == -1)
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%T)%@\n"),
            ACE_TEXT("(%T)connect")), 1);

    CPUPriorityRTFifo* cpuQoS = new CPUPriorityRTFifo(10);
    //ThreadReserveRequestPacket(ACE_CString& cpuGroup,UInt requestID,CPUQoS& qos);
    ACE_CString group("runtime_1");
    HadesPacket* packet = 0;//new ThreadReserveRequestPacket(0,group,0,cpuQoS);
    getchar();
    client.send(packet);    
    delete packet;
    ACE_Reactor::instance()->run_reactor_event_loop();
    return (0);
}

