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
 * File:   AbstractChannel.cpp
 * Author: rmartins
 * 
 * Created on December 4, 2009, 11:29 AM
 */

#include <euryale/net/AbstractDatagramChannel.h>

template <>
int AbstractDatagramChannel<ACE_SOCK_Dgram, ACE_MT_SYNCH>::open(const ACE_INET_Addr &local,
                                                                int protocol_family, int protocol, int reuse_addr) {
    if (m_peer.open(local, protocol_family, protocol, reuse_addr) != 0) {
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Error: AbstractDatagramChannel<ACE_SOCK_Dgram, ACE_MT_SYNCH>::open() -1\n")));
        return -1;
    }    
    int ret = 0;
    if (reactor() != 0) {
        if (m_asyncWrite) {
            this->m_notifier.reactor(this->reactor());
            this->m_outputQueue.notification_strategy(&m_notifier);
        }
        if (m_asyncRead) {
            ret = this->reactor()->register_handler
                (this, ACE_Event_Handler::READ_MASK);            
        }
    }
    return ret;
}

template <>
int AbstractDatagramChannel<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>::open(const ACE_INET_Addr &local,
                                                                      int protocol_family, int protocol, int reuse_addr) {
    return -1;
}

template <>
int AbstractDatagramChannel<ACE_SOCK_Dgram, ACE_MT_SYNCH>::open(const ACE_INET_Addr &mcast_addr,
                                                                const ACE_TCHAR *net_if, int reuse_addr) {
    return -1;
}

template <>
int AbstractDatagramChannel<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>::open(const ACE_INET_Addr &mcast_addr,
                                                                      const ACE_TCHAR *net_if, int reuse_addr) {
    if (m_mcast != 0) {
        delete m_mcast;
    }
    m_mcast = new ACE_INET_Addr(mcast_addr);
    //This isn't nee because join does this.
    /*if (m_peer.open(mcast_addr, net_if, reuse_addr) != 0) {
        return -1;
    }*/
    
    if (m_peer.join(mcast_addr, reuse_addr, net_if) == -1) {
        perror("ERROR: AbstractDatagramChannel<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>::join() -1\n");
        return -1;
    }
    //Cannot disable this, otherwise we don't get the packets from 
    //other runtimes that are running in the same physical host (and interface)
    /*char loopch = 1;
    if (ACE_OS::setsockopt(m_peer.get_handle(), IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loopch, sizeof (loopch)) == -1) {
        if (setsockopt(m_peer.get_handle(), IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loopch, sizeof (loopch)) == -1) {            
            return -1;
        }
    }*/
   
    int ret = 0;
    if (reactor() != 0) {
        if (m_asyncWrite) {
            this->m_notifier.reactor(this->reactor());
            this->m_outputQueue.notification_strategy(&m_notifier);
        }
        if (m_asyncRead) {
            ret = this->reactor()->register_handler
                (this, ACE_Event_Handler::READ_MASK);            
        }
    }    
    return ret;
}

