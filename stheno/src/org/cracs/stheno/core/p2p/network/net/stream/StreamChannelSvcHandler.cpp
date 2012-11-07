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
 * File:   StreamChannelSvcHandler.cpp
 * Author: rmartins
 * 
 * Created on October 27, 2010, 11:12 AM
 */

#include "StreamChannelSvcHandler.h"

StreamChannelSvcHandler::StreamChannelSvcHandler(
        StreamChannelPacketFactory* factory,
        NetReservation* reserve,
        NetQoS* qos) :
SuperType(factory,reserve, qos) {

}

/*StreamChannelSvcHandler::StreamChannelSvcHandler(
        StreamChannelPacketFactory* factory,
        PacketHandler* readHandler,
        AsyncPacketWriterHandler* writeHandler,
        bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListenerType* closeListener) :
SuperType(factory,readHandler, writeHandler, asyncRead, asyncWrite, ec, reserve, qos, closeListener) {

}*/

StreamChannelSvcHandler::StreamChannelSvcHandler(
        StreamChannelPacketFactory* factory,
        PacketHandler* readHandler,
        bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListenerType* closeListener) :
SuperType(factory,readHandler, asyncWrite, ec, reserve, qos, closeListener) {

}

StreamChannelSvcHandler::~StreamChannelSvcHandler() {
}
