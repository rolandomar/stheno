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
 * File:   NetworkUtils.cpp
 * Author: rmartins
 * 
 * Created on July 29, 2010, 2:38 PM
 */

#include "NetworkUtils.h"

NetworkUtils::NetworkUtils() {
}

NetworkUtils::NetworkUtils(const NetworkUtils& orig) {
}

NetworkUtils::~NetworkUtils() {
}

unsigned short NetworkUtils::randomPort() {
    //u_int seed = (u_int)ACE_OS::gettimeofday().msec();
    //ACE_OS::rand_r(seed);
    unsigned short port = ACE_OS::rand() % MAX_PORT;
    //printf("randomPort %d %d\n",port,MAX_PORT);
    while (port < 2000) {
        //printf("randomPort %d %d\n",port,MAX_PORT);
        port = ACE_OS::rand() % MAX_PORT;
    }
    return port;
}

int NetworkUtils::get_ip_interfaces_getifaddrs_(size_t &count,
        ACE_INET_Addr *&addrs) {
    return ACE::get_ip_interfaces(count, addrs);
}

int NetworkUtils::get_ip_interfaces_getifaddrs(size_t &count,
        ACE_INET_Addr *&addrs) {
    // Take advantage of the BSD getifaddrs function that simplifies
    // access to connected interfaces.
    struct ifaddrs *ifap = 0;
    struct ifaddrs *p_if = 0;

    if (::getifaddrs(&ifap) != 0)
        return -1;

    // Count number of interfaces.
    size_t num_ifs = 0;
    for (p_if = ifap; p_if != 0; p_if = p_if->ifa_next)
        ++num_ifs;

    // Now create and initialize output array.
    ACE_NEW_RETURN(addrs,
            ACE_INET_Addr[num_ifs],
            -1); // caller must free

    // Pull the address out of each INET interface.  Not every interface
    // is for IP, so be careful to count properly.  When setting the
    // INET_Addr, note that the 3rd arg (0) says to leave the byte order
    // (already in net byte order from the interface structure) as is.
    count = 0;

    for (p_if = ifap;
            p_if != 0;
            p_if = p_if->ifa_next) {
        if (p_if->ifa_addr &&
                p_if->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr =
                    reinterpret_cast<sockaddr_in *> (p_if->ifa_addr);

            // Sometimes the kernel returns 0.0.0.0 as the interface
            // address, skip those...
            if (addr->sin_addr.s_addr != INADDR_ANY) {
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)JJJJJJJJJJJJJJJJ=%s\n", p_if->ifa_name);
                addrs[count].set((u_short) 0,
                        addr->sin_addr.s_addr,
                        0);
                ++count;
            }
        }
#if defined (ACE_HAS_IPV6)
else if (p_if->ifa_addr &&
                p_if->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *addr =
                    reinterpret_cast<sockaddr_in6 *> (p_if->ifa_addr);

            // Skip the ANY address
            if (!IN6_IS_ADDR_UNSPECIFIED(&addr->sin6_addr)) {
                addrs[count].set(reinterpret_cast<struct sockaddr_in *> (addr),
                        sizeof (sockaddr_in6));
                ++count;
            }
        }
#endif /* ACE_HAS_IPV6 */
    }

    ::freeifaddrs(ifap);

    return 0;
}

ACE_INET_Addr* NetworkUtils::get_ip_interface(const char* itf) {
    // Take advantage of the BSD getifaddrs function that simplifies
    // access to connected interfaces.
    struct ifaddrs *ifap = 0;
    struct ifaddrs *p_if = 0;

    if (::getifaddrs(&ifap) != 0) {
        return 0;
    }

    for (p_if = ifap;
            p_if != 0;
            p_if = p_if->ifa_next) {
        if (p_if->ifa_addr &&
                p_if->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr =
                    reinterpret_cast<sockaddr_in *> (p_if->ifa_addr);

            // Sometimes the kernel returns 0.0.0.0 as the interface
            // address, skip those...
            if (addr->sin_addr.s_addr != INADDR_ANY) {
                //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)JJJJJJJJJJJJJJJJ=%s\n", p_if->ifa_name);
                if (strcmp(p_if->ifa_name, itf) == 0) {
                    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)JJJJJJJJJJJJJJJJ EHEHEHEHEH=%s\n", p_if->ifa_name);
                    ACE_INET_Addr* addrItf = new ACE_INET_Addr();
                    addrItf->set((u_short) 0,
                            addr->sin_addr.s_addr,
                            0);
                    ::freeifaddrs(ifap);
                    return addrItf;
                }
            }
        }
    }
    ::freeifaddrs(ifap);
    return 0;
}
