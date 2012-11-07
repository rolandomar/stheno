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
 * File:   UpdateInfoPacket.h
 * Author: rmartins
 *
 * Created on September 29, 2010, 6:00 PM
 */

#ifndef UPDATEINFOPACKET_H
#define	UPDATEINFOPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>

class UpdateInfoPacket : public SthenoPacket {
public:

    UpdateInfoPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong requestID,
            ACE_Message_Block* state) : 
    SthenoPacket(srcPID, srcFID, dstPID, dstFID, UPDATEINFO_MESH_PACKET, requestID),
    m_state(state) {
    }

    UpdateInfoPacket() : SthenoPacket(), m_state(0) {
    }

    virtual ~UpdateInfoPacket();

    virtual void serialize(OutputStream& outputStream) THROW(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    ACE_Message_Block* deattachUpdateInfo() {
        ACE_Message_Block* state = m_state;
        m_state = 0;
        return state;
    }

protected:
    ACE_Message_Block* m_state;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
        if (m_state != 0) {
            outputStream.write_boolean(true);
            UInt size = m_state->total_length();
            outputStream.write_ulong(size);            
            outputStream.write_octet_array_mb(m_state);
        } else {
            outputStream.write_boolean(false);
        }

    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {        
        Boolean flag = false;        
        inputStream.read_boolean(flag);
        if (flag) {
            UInt count = 0;
            inputStream.read_ulong(count);            
            if (m_state != 0) {
                delete m_state;
            }
            m_state = new ACE_Message_Block(count);
            inputStream.read_octet_array((Octet*) m_state->base(), count);
            m_state->wr_ptr(count);
        }
    }
};


#endif	/* UPDATEINFOPACKET_H */

