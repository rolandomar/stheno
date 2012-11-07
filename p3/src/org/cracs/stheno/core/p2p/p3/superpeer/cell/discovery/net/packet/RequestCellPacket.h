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
 * File:   RequestCellPacket.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 27, 2010, 11:21 AM
 */

#ifndef REQUESTCELLPACKET_H
#define	REQUESTCELLPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/P3DiscoveryPackets.h>

class RequestCellPacket : public SthenoPacket {
public:

    RequestCellPacket(UUIDPtr& srcPID,
                      UUIDPtr& srcFID,
                      UUIDPtr& dstPID,
                      UUIDPtr& dstFID,
                      ULong requestID);
    RequestCellPacket();
    virtual ~RequestCellPacket();

    virtual void serialize(OutputStream& outputStream) throw(SerializationException&);
    
protected:

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};

#endif	/* REQUESTCELLPACKET_H */

