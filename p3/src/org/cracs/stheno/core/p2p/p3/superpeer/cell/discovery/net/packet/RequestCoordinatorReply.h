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
 * File:   RequestCoordinatorReply.h
 * Author: rmartins
 *
 * Created on September 15, 2010, 2:47 PM
 */

#ifndef REQUESTCOORDINATORREPLY_H
#define	REQUESTCOORDINATORREPLY_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/P3DiscoveryPackets.h>
#include <euryale/net/endpoint/SAPInfo.h>

class RequestCoordinatorReplyPacket : public SthenoPacket {
public:

    RequestCoordinatorReplyPacket(
            const UUIDPtr& srcPID,
            const UUIDPtr& srcFID,
            const UUIDPtr& dstPID,
            const UUIDPtr& dstFID,
            ULong requestID,
            const UUIDPtr& uuid,
            const UUIDPtr& cellID,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& FTSAP,
            EndpointPtr& coordinatorEndpoint);

    RequestCoordinatorReplyPacket();
    virtual ~RequestCoordinatorReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw (SerializationException&);

    UUIDPtr& getPID();

    UUIDPtr& getCellID();

    SAPInfoPtr& getMeshSAP();

    SAPInfoPtr& getDiscoverySAP();

    SAPInfoPtr& getFTSAP();
    
    EndpointPtr& getCoordinatorEndpoint();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_cellID;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_FTSAP;
    EndpointPtr m_coordinatorEndpoint;

    virtual void serializeBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeBody(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* REQUESTCOORDINATORREPLY_H */

