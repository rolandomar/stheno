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
 * File:   CellInitSession.h
 * Author: rmartins
 *
 * Created on December 10, 2010, 11:43 AM
 */

#ifndef CELLINITSESSION_H
#define	CELLINITSESSION_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class CellInitSession : public SthenoPacket {
public:
    static const UShort CELL_INIT_SESSION_PACKET_OP = 0x000288;
    CellInitSession();
    CellInitSession(InputStream& inputStream);
    CellInitSession(
            UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong requestID          
            );
    virtual ~CellInitSession();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

protected:

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* CELLINITSESSION_H */

