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
 * File:   P3MeshCreateServicePacket.h
 * Author: rmartins
 *
 * Created on October 1, 2010, 9:47 AM
 */

#ifndef P3MESHCREATESERVICEPACKET_H
#define	P3MESHCREATESERVICEPACKET_H

#include <stheno/core/p2p/net/packet/SthenoStreamPacket.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/service/ServiceParams.h>

class P3MeshCreateServicePacket : public SthenoStreamPacket {
public:
    static const UShort P3MESH_CREATESERVICE_PACKET = 0x2397;
    P3MeshCreateServicePacket();
    P3MeshCreateServicePacket(InputStream& inputStream);
    P3MeshCreateServicePacket(ServiceParamsPtr& params);
    virtual ~P3MeshCreateServicePacket();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);

    ServiceParamsPtr& getServiceParams();


protected:
    ServiceParamsPtr m_params;

    virtual void
    serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void
    deserializeBody(InputStream& inputStream) throw (SerializationException&);
};

#endif	/* P3MESHCREATESERVICEPACKET_H */

