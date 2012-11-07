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
 * File:   JoinMeshPacket.h
 * Author: rmartins
 *
 * Created on August 26, 2010, 12:04 PM
 */

#ifndef JOINMESHPACKET_H
#define	JOINMESHPACKET_H


#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/endpoint/SAPInfo.h>

class JoinMeshPacket : public SthenoPacket {
public:

    JoinMeshPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong requestID,
            int peerType,
            UUIDPtr& uuid,
            CellIDPtr& cellID,
            CellIDPtr& parentCellID,
            EndpointPtr& endpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP
            ) : SthenoPacket(srcPID, srcFID, dstPID, dstFID, JOIN_MESH_PACKET, requestID),
    m_parentCellID(parentCellID),
    m_type(peerType),
    m_uuid(uuid),
    m_cellID(cellID),
    m_endpoint(endpoint),
    m_discoverySAP(discoverySAP),
    m_meshSAP(meshSAP),
    m_ftSAP(ftSAP) {
    }

    JoinMeshPacket() : SthenoPacket(),
    m_parentCellID(0),
    m_type(0),
    m_uuid(0),
    m_cellID(0),
    m_endpoint(0),
    m_discoverySAP(0),
    m_meshSAP(0),
    m_ftSAP(0)
    {
    }

    virtual ~JoinMeshPacket();

    virtual void serialize(OutputStream& outputStream) THROW(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);
    }

    CellIDPtr& getParentCellID() {
        return m_parentCellID;
    }

    UUIDPtr& getUUID() {
        return m_uuid;
    }

    CellIDPtr& getCellID() {
        return m_cellID;
    }

    EndpointPtr& getEndpoint() {
        return m_endpoint;
    }

    SAPInfoPtr& getDiscoverySAP() {
        return m_discoverySAP;
    }

    SAPInfoPtr& getMeshSAP() {
        return m_meshSAP;
    }

    SAPInfoPtr& getFTSAP() {
        return m_ftSAP;
    }

    int getType() {
        return m_type;
    }


protected:
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;
    CellIDPtr m_parentCellID;
    EndpointPtr m_endpoint;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_ftSAP;

    int m_type;
    //JoinPeer m_joinPeer;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
        outputStream.write_long(m_type);
        m_uuid->serialize(outputStream);
        m_cellID->serialize(outputStream);
        if (!m_parentCellID.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_parentCellID->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }

        if (!m_endpoint.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_endpoint->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }

        if (!m_discoverySAP.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_discoverySAP->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }

        if (!m_meshSAP.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_meshSAP->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }

        if (!m_ftSAP.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_ftSAP->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
        Boolean flag = false;
        inputStream.read_long(m_type);
        UUID* uuid = new UUID(inputStream);
        m_uuid.reset(uuid);
        CellID* cellID = new CellID(inputStream);
        m_cellID.reset(cellID);

        inputStream.read_boolean(flag);
        if (flag) {
            CellID* parentCellID = new CellID(inputStream);
            m_parentCellID.reset(parentCellID);
        }
        inputStream.read_boolean(flag);
        if (flag) {
            Endpoint* endpoint = new Endpoint(inputStream);
            m_endpoint.reset(endpoint);
        }
        inputStream.read_boolean(flag);
        if (flag) {
            SAPInfo* info = new SAPInfo(inputStream);
            m_discoverySAP.reset(info);
        }

        inputStream.read_boolean(flag);
        if (flag) {
            SAPInfo* info = new SAPInfo(inputStream);
            m_meshSAP.reset(info);
        }

        inputStream.read_boolean(flag);
        if (flag) {
            SAPInfo* info = new SAPInfo(inputStream);
            m_ftSAP.reset(info);
        }
    }
};


#endif	/* JOINMESHPACKET_H */

