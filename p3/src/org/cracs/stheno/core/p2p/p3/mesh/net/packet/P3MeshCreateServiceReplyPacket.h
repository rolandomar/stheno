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
 * File:   P3MeshCreateServiceReplyPacket.h
 * Author: rmartins
 *
 * Created on October 1, 2010, 9:48 AM
 */

#ifndef P3MESHCREATESERVICEREPLYPACKET_H
#define	P3MESHCREATESERVICEREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/mesh/exception/MeshException.h>

class P3MeshCreateServiceReplyPacket : public SthenoStreamPacket {
public:
    static const UShort P3MESH_CREATESERVICERPELY_PACKET = 0x7212;
    P3MeshCreateServiceReplyPacket();    
    P3MeshCreateServiceReplyPacket(InputStream& inputStream);
    P3MeshCreateServiceReplyPacket(UInt replyPacketID,UUIDPtr& iid);
    P3MeshCreateServiceReplyPacket(UInt replyPacketID,MeshException* exception);    
    virtual ~P3MeshCreateServiceReplyPacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    bool isException();
    MeshException* getException();
    UUIDPtr& getIID();
    
    UInt getReplyPacketID(){
        return m_replyPacketID;
    }
    
protected:
    UInt m_replyPacketID;
    UUIDPtr m_iid;
    MeshException* m_exception;

    virtual void serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* P3MESHCREATESERVICEREPLYPACKET_H */

