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
 * PacketBase.cpp
 *
 *  Created on: May 18, 2008
 *      Author: rmartins
 */

#include <euryale/qos/net/packet/MedusaPacket.h>
namespace pt {

namespace efacec {

namespace rto {

namespace net {

namespace packet {

MedusaHeader::MedusaHeader() :
        m_magic(RTO_MAGIC), m_protocol(0), m_protocolVersionMajor(0),
        m_protocolVersionMinor(0), m_messageType(0), 
        m_requestID(0), m_messageSize(0) {
}

MedusaHeader::MedusaHeader(UShort messageType,
        ULong requestID, ULong messageSize) :
        m_magic(RTO_MAGIC), m_protocol(0), m_protocolVersionMajor(0),
        m_protocolVersionMinor(1), m_messageType(messageType),
        m_requestID(requestID),       
        m_messageSize(0) {

}

MedusaHeader::MedusaHeader(ULong magic,
        UShort protocol, Byte protocolVersionMajor, Byte protocolVersionMinor,
        UShort messageType, ULong requestID,
        ULong messageSize) :
        m_magic(magic), m_protocol(protocol),
        m_protocolVersionMajor(protocolVersionMajor),
        m_protocolVersionMinor(protocolVersionMinor),
        m_messageType(messageType), m_requestID(requestID),
        m_messageSize(messageSize){
}

MedusaHeader::~MedusaHeader() {
}

}

}

}

}

}
