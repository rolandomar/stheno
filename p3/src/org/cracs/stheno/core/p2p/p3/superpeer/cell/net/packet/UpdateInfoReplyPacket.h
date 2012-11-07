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
 * File:   UpdateInfoReplyPacket.h
 * Author: rmartins
 *
 * Created on September 29, 2010, 6:01 PM
 */

#ifndef UPDATEINFOREPLYPACKET_H
#define	UPDATEINFOREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>

class UpdateInfoReplyPacket : public SthenoPacket {
public:
   
    UpdateInfoReplyPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong packetID,
            ULong replyPacketID,
            bool updateStatus) :
    SthenoPacket(srcPID, srcFID, dstPID, dstFID, UPDATEINFO_MESH_REPLY_PACKET, packetID),
    m_updateStatus(updateStatus),m_replyPacketID(replyPacketID){}

    UpdateInfoReplyPacket() : m_updateStatus(false),m_replyPacketID(0),
    SthenoPacket() {}

    virtual ~UpdateInfoReplyPacket();

    virtual void serialize(OutputStream& outputStream) throw(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    bool& getUpdateResult() {
        return m_updateStatus;
    }

    ULong getReplyPacketID(){
        return m_replyPacketID;
    }

protected:
    ULong m_replyPacketID;
    Boolean m_updateStatus;    

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&) {        
        outputStream.write_ulong(m_replyPacketID);
        outputStream.write_boolean(m_updateStatus);
    }

    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&) {        
        inputStream.read_ulong(m_replyPacketID);
        inputStream.read_boolean(m_updateStatus);
    }
};

#endif	/* UPDATEINFOREPLYPACKET_H */

