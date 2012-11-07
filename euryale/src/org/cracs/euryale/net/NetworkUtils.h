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
 * File:   NetworkUtils.h
 * Author: rmartins
 *
 * Created on July 29, 2010, 2:38 PM
 */

#ifndef NETWORKUTILS_H
#define	NETWORKUTILS_H


#include /**/ <ifaddrs.h>
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_sys_socket.h"
#include "ace/OS_NS_netdb.h"
#include "ace/OS_NS_unistd.h"
#include "ace/os_include/net/os_if.h"
#include "ace/Sock_Connect.h"
#include "ace/INET_Addr.h"
#include "ace/Log_Msg.h"
#include "ace/Handle_Set.h"
#include "ace/Auto_Ptr.h"
#include "ace/SString.h"
#include "ace/OS_Memory.h"
#include "ace/OS_NS_stdio.h"
#include "ace/ACE.h"

class NetworkUtils {
public:
    
    static const unsigned short MAX_PORT = 65534;
    
    NetworkUtils();
    NetworkUtils(const NetworkUtils& orig);
    virtual ~NetworkUtils();
    
    static unsigned short randomPort();

    static int get_ip_interfaces_getifaddrs_(size_t &count,ACE_INET_Addr *&addrs);

    static int get_ip_interfaces_getifaddrs(size_t &count,ACE_INET_Addr *&addrs);

    static ACE_INET_Addr* get_ip_interface(const char* itf);
};

#endif	/* NETWORKUTILS_H */

