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
 * File:   LeaveMeshReplyPacket.h
 * Author: rmartins
 *
 * Created on September 1, 2010, 11:19 AM
 */

#ifndef LEAVEMESHREPLYPACKET_H
#define	LEAVEMESHREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>

class LeaveMeshReplyPacket : public SthenoPacket {
public:

    LeaveMeshReplyPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong packetID,
            ULong requestID,
            bool acception) :
    SthenoPacket(srcPID, srcFID, dstPID, dstFID, LEAVE_MESH_REPLY_PACKET, packetID),
    m_acception(acception),m_requestID(requestID){

    }

    LeaveMeshReplyPacket() : m_acception(false),SthenoPacket() {
    }

    virtual ~LeaveMeshReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    bool& getLeaveResult() {
        return m_acception;
    }
    
    ULong getRequestID(){
        return m_requestID;
    }

protected:
    ULong m_requestID;
    Boolean m_acception;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
        outputStream.write_ulong(m_requestID);
        outputStream.write_boolean(m_acception);
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
        inputStream.read_ulong(m_requestID);
        inputStream.read_boolean(m_acception);
    }
};

#endif	/* LEAVEMESHREPLYPACKET_H */

