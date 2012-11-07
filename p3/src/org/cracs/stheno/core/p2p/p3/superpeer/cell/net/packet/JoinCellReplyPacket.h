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
 * File:   JoinCellReplyPacket.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 5:53 PM
 */

#ifndef JOINCELLREPLYPACKET_H
#define	JOINCELLREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>

class JoinCellReplyPacket : public SthenoPacket {
public:

    JoinCellReplyPacket(UUIDPtr& srcPID,
                        UUIDPtr& srcFID,
                        UUIDPtr& dstPID,
                        UUIDPtr& dstFID,
                        ULong packetID,
                        ULong requestID) :
    SthenoPacket(srcPID, srcFID, dstPID, dstFID, JOIN_CELL_REPLY_PACKET, packetID),
    m_state(0), m_requestID(requestID) {

    }

    JoinCellReplyPacket(UUIDPtr& srcPID,
                        UUIDPtr& srcFID,
                        UUIDPtr& dstPID,
                        UUIDPtr& dstFID,
                        ULong packetID,
                        ULong requestID,
                        ACE_Message_Block* state) :
    SthenoPacket(srcPID, srcFID, dstPID, dstFID, JOIN_CELL_REPLY_PACKET, packetID),
    m_state(state), m_requestID(requestID) {

    }

    JoinCellReplyPacket() : SthenoPacket(),m_state(0) {  
        if(m_state != 0){
            ACE_Message_Block::release(m_state);        
        }
    }

    virtual ~JoinCellReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&) {        
        UInt bodySize = getBodySerializationSize();        
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    bool getStatus() {
        return (m_state!=0);
    }

    ACE_Message_Block* deattachState() {
        ACE_Message_Block* state = m_state;
        m_state = 0;
        return state;
    }

    ULong getRequestID() {
        return m_requestID;
    }

protected:
    ULong m_requestID;    
    ACE_Message_Block* m_state;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {        
        outputStream.write_ulong(m_requestID);        
        if (m_state!=0) {
            outputStream.write_boolean(true);
            UInt size = m_state->total_length();            
            outputStream.write_ulong(size);            
            outputStream.write_octet_array_mb(m_state);            
        }else{
            outputStream.write_boolean(false);
        }
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {        
        inputStream.read_ulong(m_requestID);        
        Boolean flag = false;
        inputStream.read_boolean(flag);
        if (flag!= 0) {
            UInt count = 0;
            inputStream.read_ulong(count);
            if (m_state != 0) {
                //delete m_state;
                ACE_Message_Block::release(m_state);
            }           
            m_state = new ACE_Message_Block(count);
            inputStream.read_octet_array((Octet*) m_state->base(), count);
            m_state->wr_ptr(count);
        }
    }
};

#endif	/* JOINCELLREPLYPACKET_H */

