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
 * File:   CellReply.h
 * Author: rmartins
 *
 * Created on June 12, 2010, 2:44 PM
 */

#ifndef CELLREPLY_H
#define	CELLREPLY_H

#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <euryale/Types.h>
#include <ace/INET_Addr.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellAddr.h>
#include <euryale/net/endpoint/SAPInfo.h>

class CellReply {
public:
    //fast conversion from packet
    CellReply(
            CellAddrPtr& cell,
            UUIDPtr& uuid,
            EndpointPtr& addr,
            CellAddrPtr& parentCell,
            UUIDPtr& parentUUID,
            EndpointPtr& parentAddr,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP);

    //coordinator@cellID and connect to the parent coordinator
    CellReply(
            CellAddrPtr& cell,
            CellAddrPtr& parentCell,
            UUIDPtr& parentUUID,
            EndpointPtr& parentAddr,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP);
    //not-coordinator, join a cell using a coordinator
    CellReply(
            CellAddrPtr& cell,
            UUIDPtr& uuid,
            EndpointPtr& addr,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP);
    //coordinator root case
    CellReply(
            CellAddrPtr& rootCell,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP);

    virtual ~CellReply();
    //assigned cell
    bool hasCoordinator();
    UUIDPtr& getCoordinatorUUID();
    CellAddrPtr& getCell();
    EndpointPtr& getCoordinatorAddr();

    //parent cell
    bool hasParentCoordinator();
    bool hasParentCellID();
    CellAddrPtr& getParentCell();
    EndpointPtr& getParentCoordinatorAddr();
    UUIDPtr& getParentUUID();

    SAPInfoPtr& getDiscoverySAPInfo();
    SAPInfoPtr& getFTSAPInfo();
    SAPInfoPtr& getMeshSAPInfo();


protected:
    CellAddrPtr m_cell;
    CellAddrPtr m_parentCell;
    //cell coordinator
    UUIDPtr m_uuid;
    EndpointPtr m_addr;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_ftSAP;
    //parent info
    UUIDPtr m_parentUUID;
    EndpointPtr m_parentAddr;




};

#endif	/* CELLREPLY_H */

