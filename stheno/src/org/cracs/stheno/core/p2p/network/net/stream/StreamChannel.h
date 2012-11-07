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
 * File:   StreamChannel.h
 * Author: rmartins
 *
 * Created on November 1, 2010, 12:31 PM
 */

#ifndef STREAMCHANNEL_H
#define	STREAMCHANNEL_H

#include <euryale/net/PacketChannel.h>
#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <stheno/core/p2p/network/net/stream/packet/StreamChannelPacketFactory.h>
#include <euryale/net/ChannelRequestEngine.h>
#include <stheno/core/p2p/network/net/stream/AsyncPacketReaderHandler.h>



class StreamChannel: public PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
    /*StreamChannelPacketFactory,*/ ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    
    typedef PacketChannel<SthenoStreamPacket, SthenoStreamHeader,
        /*StreamChannelPacketFactory,*/ ACE_SOCK_Stream, ACE_MT_SYNCH> SuperType;
    typedef SuperType::CloseListenerType CloseListenerType;
    
    StreamChannel();
    //Sync socket constructor
    StreamChannel(           
            StreamChannelPacketFactory* factory,
            NetReservation* reserve,
            NetQoS* qos);
    
    StreamChannel(
            StreamChannelPacketFactory* factory,
            PacketHandler* readHandler,            
            bool asyncWrite,
            ExecutionModelPtr* ec,
            NetReservation* reserve = 0,
            NetQoS* qos = 0,
            CloseListenerType* closeListener = 0);
    
    
    virtual ~StreamChannel();
        
    UUIDPtr& getPID() {
        return m_uuid;
    }

    UUIDPtr& getFID() {
        return m_fid;
    }

    QoSEndpoint& getQoSEndpoint() {
        return m_channelQoS;
    }
    
    virtual int asynchronous(PacketHandler* packetHandler,bool asyncWrite){
        bool asyncRead = true;
        m_readHandler = packetHandler;
        if(packetHandler==0){
            asyncRead = false;
        }
        return SuperType::asynchronous(asyncRead,asyncWrite);
    }
    
protected:
    PacketHandler* m_readHandler;
    
        
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    QoSEndpoint m_channelQoS;
    
    
    virtual int handlePacket(SthenoStreamPacket* packet);

};

#endif	/* STREAMCHANNEL_H */

