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
 * File:   P3MeshRemoveServicePacket.h
 * Author: rmartins
 *
 * Created on October 1, 2010, 9:48 AM
 */

#ifndef P3MESHREMOVESERVICEPACKET_H
#define	P3MESHREMOVESERVICEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceParams.h>

class P3MeshRemoveServicePacket : public SthenoStreamPacket {
public:
    static const UShort P3MESH_REMOVESERVICE_PACKET = 0x8612;
    P3MeshRemoveServicePacket();
    P3MeshRemoveServicePacket(InputStream& inputStream);
    P3MeshRemoveServicePacket(UUIDPtr sid,UUIDPtr iid);
    virtual ~P3MeshRemoveServicePacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getSID();
    UUIDPtr& getIID();

protected:
    UUIDPtr m_sid;
    UUIDPtr m_iid;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* P3MESHREMOVESERVICEPACKET_H */

