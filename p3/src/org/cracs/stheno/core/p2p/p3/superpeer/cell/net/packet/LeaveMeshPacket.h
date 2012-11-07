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
 * File:   LeaveMeshPacket.h
 * Author: rmartins
 *
 * Created on September 1, 2010, 11:11 AM
 */

#ifndef LEAVEMESHPACKET_H
#define	LEAVEMESHPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>
#include <euryale/net/endpoint/Endpoint.h>

class LeaveMeshPacket : public SthenoPacket {
public:

    LeaveMeshPacket(UUIDPtr& srcPID,
                   UUIDPtr& srcFID,
                   UUIDPtr& dstPID,
                   UUIDPtr& dstFID,
                   ULong requestID,                   
                   UUIDPtr& uuid,
                   CellIDPtr& cellID
                   ) : SthenoPacket(srcPID, srcFID, dstPID, dstFID, LEAVE_MESH_PACKET, requestID),        
    m_uuid(uuid),
    m_cellID(cellID){}


    LeaveMeshPacket() : SthenoPacket() {
    }

    virtual ~LeaveMeshPacket();

    virtual void serialize(OutputStream& outputStream) THROW(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    UUIDPtr& getUUID(){
        return m_uuid;
    }

    CellIDPtr& getCellID(){
        return m_cellID;
    }
    
protected:
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
        m_uuid->serialize(outputStream);
        m_cellID->serialize(outputStream);
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
        UUID* uuid = new UUID(inputStream);
        m_uuid.reset(uuid);
        CellID* cellID = new CellID(inputStream);
        m_cellID.reset(cellID);
    }
};

#endif	/* LEAVEMESHPACKET_H */

