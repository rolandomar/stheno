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
 * File:   SthenoPacketFactory.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 4, 2010, 5:10 PM
 */

#ifndef STHENOPACKETFACTORY_H
#define	STHENOPACKETFACTORY_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <euryale/common/io/ByteInputStream.h>

//#include <stheno/core/p2p/p3/discovery/net/packet/P3DiscoveryPackets.h>
//#include <stheno/core/p2p/p3/discovery/net/packet/RequestCellPacket.h>
//include <stheno/core/p2p/p3/discovery/net/packet/RequestCellReplyPacket.h>

class SthenoPacketFactory {
public:
    SthenoPacketFactory(){}
    virtual ~SthenoPacketFactory(){}


    static SthenoPacket* createPacket(SthenoHeader* header, ByteInputStream& is){
      /*switch(header->m_messageType){
            case REQUEST_CELL_PACKET:{
                RequestCellPacket* packet = new RequestCellPacket();
                packet->init(header,is);
                return packet;
            }
            case REQUEST_CELL_REPLY_PACKET:{
                RequestCellReplyPacket* packet = new RequestCellReplyPacket();
                packet->init(header,is);
                return packet;
            }
            default:
                return 0;
		}*/
        return 0;
	}
private:

};

#endif	/* STHENOPACKETFACTORY_H */

