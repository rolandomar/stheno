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
 * File:   StreamChannel.cpp
 * Author: rmartins
 * 
 * Created on November 1, 2010, 12:31 PM
 */

#include "StreamChannel.h"

StreamChannel::StreamChannel() :
SuperType(), m_readHandler(0) {

}

StreamChannel::StreamChannel(
        StreamChannelPacketFactory* factory,
        NetReservation* reserve,
        NetQoS* qos) :
SuperType(factory, 0, false, 0, reserve, qos, 0), m_readHandler(0) {
}

StreamChannel::StreamChannel(
        StreamChannelPacketFactory* factory,
        PacketHandler* readHandler,
        bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListenerType* closeListener) :
SuperType(factory, false, asyncWrite, ec, reserve, qos, closeListener),m_readHandler(readHandler) {
    if (readHandler != 0) {
        SuperType::m_asyncRead = true;
    }
}

StreamChannel::~StreamChannel() {
}

int StreamChannel::handlePacket(SthenoStreamPacket* packet) {
    return m_readHandler->handlePacket(this, packet);
}
