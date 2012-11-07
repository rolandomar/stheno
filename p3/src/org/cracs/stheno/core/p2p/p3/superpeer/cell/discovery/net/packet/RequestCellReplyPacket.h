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
 * File:   RequestCellReplyPacket.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 27, 2010, 2:21 PM
 */

#ifndef REQUESTCELLREPLYPACKET_H
#define	REQUESTCELLREPLYPACKET_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/discovery/net/packet/P3DiscoveryPackets.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellAddr.h>
#include <euryale/net/endpoint/SAPInfo.h>

class RequestCellReplyPacket : public SthenoPacket {
public:

    RequestCellReplyPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong requestID,
            //assigned cell
            CellAddrPtr& cell,
            UUIDPtr& coordinatorUUID,
            EndpointPtr& coordinatorEndpoint,
            //parent cell
            CellAddrPtr& parentCell,
            UUIDPtr& parentUUID,
            EndpointPtr& parentCoordinatorEndpoint,
            //SAP (either from cell's coordinator or from parent's cell coor.)
            SAPInfoPtr& discoverySAPInfo,
            SAPInfoPtr& meshSAPInfo,
            SAPInfoPtr& ftSAPInfo
            ) ;

    RequestCellReplyPacket();
    virtual ~RequestCellReplyPacket();

    virtual void
    serialize(OutputStream& outputStream) throw(SerializationException&);

    bool hasCoordinator();
    
    CellAddrPtr& getCell();

    UUIDPtr& getCoordinatorUUID();

    EndpointPtr& getCoordinatorEndpoint();
    //Parent cell related methods

    bool hasParentCoordinator();

    bool hasParentCell();

    UUIDPtr& getParentUUID();

    CellAddrPtr& getParentCell();

    EndpointPtr& getParentCoordinatorEndpoint();

    SAPInfoPtr& getFTSAPInfo();

    SAPInfoPtr& getDiscoverySAPInfo();

    SAPInfoPtr& getMeshSAPInfo();

protected:
    CellAddrPtr m_cell;
    UUIDPtr m_coordinatorUUID;
    EndpointPtr m_coordinatorEndpoint;
    UUIDPtr m_parentUUID;
    CellAddrPtr m_parentCell;
    EndpointPtr m_parentCoordinatorEndpoint;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_ftSAPInfo;

    virtual void
    serializeBody(OutputStream& outputStream) throw(SerializationException&);

    virtual void
    deserializeBody(InputStream& inputStream) throw(SerializationException&);
};


#endif	/* REQUESTCELLREPLYPACKET_H */

