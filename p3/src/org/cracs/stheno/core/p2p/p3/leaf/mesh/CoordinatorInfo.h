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
 * File:   CoordinatorInfo.h
 * Author: rmartins
 *
 * Created on January 11, 2011, 12:45 PM
 */

#ifndef COORDINATORINFO_H
#define	COORDINATORINFO_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/SAPInfo.h>
#include <euryale/net/endpoint/Endpoint.h>

class CoordinatorInfo {
public:

    CoordinatorInfo(
            UUIDPtr uuid,
            UUIDPtr cellID,
            SAPInfoPtr meshSAP,
            SAPInfoPtr discoverySAP,
            SAPInfoPtr FTSAP,
            EndpointPtr& cellCoordEndpoint);
    CoordinatorInfo(const CoordinatorInfo& orig);
    virtual ~CoordinatorInfo();

    UUIDPtr& getPID();
    UUIDPtr& getCellID();

    SAPInfoPtr& getMeshSap();
    SAPInfoPtr& getDiscoverySap();
    SAPInfoPtr& getFTSap();    
    EndpointPtr& getCellCoordinatorEndpoint();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_cellID;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_FTSAP;
    EndpointPtr m_cellCoordEndpoint;
};

#endif	/* COORDINATORINFO_H */

