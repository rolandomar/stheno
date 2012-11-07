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
 * File:   PacketFactory.h
 * Author: rmartins
 *
 * Created on November 8, 2010, 7:52 PM
 */

#ifndef PACKETFACTORY_H
#define	PACKETFACTORY_H


#include <euryale/common/io/ByteInputStream.h>
template <class PACKET,class HEADER>
class PacketFactory {
public:
    virtual ~PacketFactory(){}
    virtual PACKET* createPacket(HEADER* header, ByteInputStream& is) = 0;
};

#endif	/* PACKETFACTORY_H */

