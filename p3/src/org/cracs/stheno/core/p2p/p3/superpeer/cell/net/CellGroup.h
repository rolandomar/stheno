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
 * File:   CellGroup.h
 * Author: rmartins
 *
 * Created on August 31, 2010, 4:33 PM
 */

#ifndef CELLGROUP_H
#define	CELLGROUP_H


#include <ace/SOCK_Stream.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/Cell.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/UpdateInfoPacket.h>

class CellGroup : public ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH> {
public:
    CellGroup(Cell* cell);
    CellGroup(UUIDPtr& uuid,CellIDPtr& cellID);
    virtual ~CellGroup();

    void joinPeer(int peerType,
            UUIDPtr& uuid,
            CellIDPtr& cellID,
            CellIDPtr& parentCellID,
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP
            );
    void rebindPeer(int peerType,
            UUIDPtr& uuid,
            CellIDPtr& cellID,
            CellIDPtr& parentCellID,                        
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP,
            ACE_Message_Block* mb);
    void leftPeer(UUIDPtr& uuid,
            CellIDPtr& cellID);

    void updateInfo(UUIDPtr& uuid,InfoUpdatePtr& updateInfo);

protected:    
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;

    UUIDPtr& getUUID(){
        return m_uuid;
    }
    
    CellIDPtr& getCellID(){
        return m_cellID;
    }

};

#endif	/* CELLGROUP_H */

