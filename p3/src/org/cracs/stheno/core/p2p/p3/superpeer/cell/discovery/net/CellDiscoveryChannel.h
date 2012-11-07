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
 * File:   CellDiscovery.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 25, 2010, 4:21 PM
 */

#ifndef CELLDISCOVERYCHANNEL_H
#define	CELLDISCOVERYCHANNEL_H

#include <stheno/core/p2p/net/dgram/mcast/SthenoMulticastChannel.h>
#include <euryale/net/RequestEngine.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/DiscoveryPacketFactory.h>

class CellDiscoveryChannel : public SthenoMulticastChannel<P3CellDiscoveryPacketFactory> {
public:

    CellDiscoveryChannel(CellDiscovery* cellDiscovery, bool asyncRead, bool asyncWrite,
            bool inputQueue = false,
            ExecutionModelPtr* ec = 0,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseDatagramListener<ACE_SOCK_Dgram_Mcast, ACE_MT_SYNCH>* closeListener = 0);

    virtual RequestEngine<SthenoPacket*>::RequestPtr* sendRequest(SthenoPacket* packet,
            ACE_Time_Value* timeout = 0);

    virtual ~CellDiscoveryChannel();

    int handlePacket(SthenoPacket* packet);

    virtual int close(u_long flags = 0) {
        {
            ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
            if (!m_closed) {
                m_requests.clearAll();
            }
        }
        return SthenoMulticastChannel<P3CellDiscoveryPacketFactory>::close(flags);
    }

protected:
    RequestEngine<SthenoPacket*> m_requests;
    CellDiscovery* m_cellDiscovery;
    bool m_debugCellDiscoveryChannel;
};

#endif	/* CELLDISCOVERYCHANNEL_H */

