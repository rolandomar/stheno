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
 * File:   P3MeshRemoveServiceReplyPacket.h
 * Author: rmartins
 *
 * Created on October 1, 2010, 9:49 AM
 */

#ifndef P3MESHREMOVESERVICEREPLYPACKET_H
#define	P3MESHREMOVESERVICEREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/mesh/exception/MeshException.h>

class P3MeshRemoveServiceReplyPacket : public SthenoStreamPacket {
public:
    static const UShort P3MESH_REMOVESERVICERPELY_PACKET = 0x3998;
    P3MeshRemoveServiceReplyPacket();
    P3MeshRemoveServiceReplyPacket(UInt replyPacketID);
    P3MeshRemoveServiceReplyPacket(InputStream& inputStream);    
    P3MeshRemoveServiceReplyPacket(UInt replyPacketID,MeshException* exception);    
    virtual ~P3MeshRemoveServiceReplyPacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    bool isException();
    MeshException* getException();    
    
    UInt getReplyPacketID(){
        return m_replyPacketID;
    }
    
protected:    
    UInt m_replyPacketID;
    MeshException* m_exception;

    virtual void serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeBody(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* P3MESHREMOVESERVICEREPLYPACKET_H */

