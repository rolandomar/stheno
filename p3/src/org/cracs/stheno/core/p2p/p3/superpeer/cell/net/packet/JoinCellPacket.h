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
 * File:   JoinCell.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on June 9, 2010, 12:36 PM
 */

#ifndef JOINCELL_H
#define	JOINCELL_H

#include <stheno/core/p2p/net/packet/SthenoPacket.h>
#include <stheno/core/p2p/p3/superpeer/cell/CellID.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/packet/CellPacketTypes.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/endpoint/SAPInfo.h>

class JoinPeer : public Serializable {
public:

    JoinPeer() {
    }

    JoinPeer(InputStream& inputStream) {
        this->deserialize(inputStream);
    }

    JoinPeer(UUIDPtr& uuid, CellIDPtr& cellID, CellIDPtr& parentCellID, SAPInfoPtr& discoverySAP) :
    m_uuid(uuid), m_cellID(cellID), m_parentCellID(parentCellID), m_discoverySAP(discoverySAP) {
    }

    JoinPeer(const JoinPeer& jp) :
    m_uuid(jp.m_uuid), m_cellID(jp.m_cellID), m_parentCellID(jp.m_parentCellID), m_discoverySAP(jp.m_discoverySAP) {

    }

    UUIDPtr& getUUID() {
        return m_uuid;
    }

    CellIDPtr& getCellID() {
        return m_cellID;
    }

    CellIDPtr& getParentCellID() {
        return m_parentCellID;
    }

    SAPInfoPtr& getDiscoverySAP() {
        return m_discoverySAP;
    }

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        m_uuid->serialize(outputStream);
        m_cellID->serialize(outputStream);
        m_parentCellID->serialize(outputStream);
        if (!m_discoverySAP.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_discoverySAP->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }

    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        UUID* uuid = new UUID(inputStream);
        m_uuid.reset(uuid);
        CellID* cellID = new CellID(inputStream);
        m_cellID.reset(cellID);
        CellID* parentCellID = new CellID(inputStream);
        m_parentCellID.reset(parentCellID);
        bool flag = false;
        inputStream.read_boolean(flag);
        if (flag) {
            SAPInfo* info = new SAPInfo(inputStream);
            m_discoverySAP.reset(info);
        }
    }


protected:
    UUIDPtr m_uuid;
    CellIDPtr m_cellID;
    CellIDPtr m_parentCellID;
    SAPInfoPtr m_discoverySAP;
};

class JoinCellPacket : public SthenoPacket {
public:

    JoinCellPacket(UUIDPtr& srcPID,
            UUIDPtr& srcFID,
            UUIDPtr& dstPID,
            UUIDPtr& dstFID,
            ULong requestID,
            EndpointPtr& endpoint,
            EndpointPtr& followerEndpoint,
            SAPInfoPtr& discoverySAP,
            SAPInfoPtr& meshSAP,
            SAPInfoPtr& ftSAP
            ) : SthenoPacket(srcPID, srcFID, dstPID, dstFID, JOIN_CELL_PACKET, requestID),
    m_endpoint(endpoint),m_followerEndpoint(followerEndpoint),m_discoverySAP(discoverySAP),
    m_meshSAP(meshSAP), m_FTSAP(ftSAP) {//,m_joinPeer(joinPeer){

    }
    //RequestCellPacket(){}

    JoinCellPacket() : SthenoPacket() {
    }

    virtual ~JoinCellPacket();

    virtual void
    serialize(OutputStream& outputStream) THROW(SerializationException&) {
        UInt bodySize = getBodySerializationSize();
        ((SthenoHeader*) m_packetHeader)->m_messageSize = bodySize;
        serializeHeader(outputStream);
        serializeBody(outputStream);

    }

    SAPInfoPtr& getFTSAP() {
        return m_FTSAP;
    }

    SAPInfoPtr& getMeshSAP() {
        return m_meshSAP;
    }

    SAPInfoPtr& getDiscoverySAP() {
        return m_discoverySAP;
    }

    EndpointPtr& getEndpoint() {
        return m_endpoint;
    }
    
    EndpointPtr& getFollowerEndpoint() {
        return m_followerEndpoint;
    }

protected:
    //JoinPeer m_joinPeer;
    EndpointPtr m_endpoint;
    EndpointPtr m_followerEndpoint;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_FTSAP;

    virtual void
    serializeBody(OutputStream& outputStream) THROW(SerializationException&) {
        //m_joinPeer.serialize(outputStream);
        m_endpoint->serialize(outputStream);
        m_followerEndpoint->serialize(outputStream);
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

        if (!m_FTSAP.null()) {
            Boolean flag = true;
            outputStream.write_boolean(flag);
            m_FTSAP->serialize(outputStream);
        } else {
            Boolean flag = false;
            outputStream.write_boolean(flag);
        }
    }

    virtual void
    deserializeBody(InputStream& inputStream) THROW(SerializationException&) {
        //m_joinPeer.deserialize(inputStream);
        Endpoint* endpoint = new Endpoint(inputStream);
        m_endpoint.reset(endpoint);
        Endpoint* followerEndpoint = new Endpoint(inputStream);
        m_followerEndpoint.reset(followerEndpoint);
        bool flag = false;
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
            m_FTSAP.reset(info);
        }
    }
};

#endif	/* JOINCELL_H */

