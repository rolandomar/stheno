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
 * File:   Network.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 20, 2010, 3:22 PM
 */

#ifndef NETWORK_H
#define	NETWORK_H

#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <stheno/core/p2p/P2PService.h>
#include <euryale/net/endpoint/Endpoint.h>

//#include <stheno/core/p2p/network/net/stream/StreamChannel.h>
//#include <stheno/core/p2p/network/net/dgram/DatagramChannel.h>
//#include <stheno/core/p2p/network/net/NetworkAcceptor.h>
#include <euryale/net/PacketChannel.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>


/*
 Frontend for network primitives.
 This service handles the QoS reservations maintainance 
 (using the overlay as a broker, that in turn coordinates with the runtime core)
 */

/*class IncomingChannelHandler {
public:
    virtual void handleIncomingChannel(StreamChannel* channel) = 0;
    virtual NetworkAcceptor* getAcceptor() = 0;
};*/

class Network : public P2PService {
public:
    /*typedef PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
    StreamChannelPacketFactory, ACE_SOCK_Stream, ACE_MT_SYNCH> StreamChannel;*/

    Network(Overlay* overlay);
    virtual ~Network();

    /* API for high level networking */
    virtual const char* id() {
        return "61D829625B148A1D5CD4720AF5E86C41";
    }

    /*StreamChannel* createStreamChannel(Endpoint& e) throw (ServiceException&) {
    }

    DatagramChannel* createDatagramChannel() throw (ServiceException&) {
    }

    //handler should have access to the acceptor
    NetworkAcceptor* createStreamSAP(HandleIncomingChannelHandler* handler, QoSEndpoint* qos) {
    }*/

private:

};

typedef ACE_Strong_Bound_Ptr<Network, ACE_Recursive_Thread_Mutex> NetworkPtr;

#endif	/* NETWORK_H */

