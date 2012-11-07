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
 * File:   TreePeerInfo.h
 * Author: rmartins
 *
 * Created on August 18, 2010, 5:29 PM
 */

#ifndef TREEPEERINFO_H
#define	TREEPEERINFO_H

#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/net/endpoint/SAPInfo.h>

class P3PeerInfo : public PeerInfo {
public:

    enum {
        SUPERPEER = 1,
        SUPERPEER_COORD = 2,
        SENSOR = 3,
        MONITOR = 4
    };

    P3PeerInfo(InputStream& inputStream);
    P3PeerInfo(
            const UUIDPtr& pid,
            const UUIDPtr& fid,
            const EndpointPtr& m_cellEndpoint,
            const SAPInfoPtr& meshSAP,
            const SAPInfoPtr& discoverySAP,
            const SAPInfoPtr& ftSAP);
    P3PeerInfo(
            UInt type,
            const UUIDPtr& pid,
            const EndpointPtr& m_cellEndpoint,
            const SAPInfoPtr& meshSAP,
            const SAPInfoPtr& discoverySAP,
            const SAPInfoPtr& ftSAP);
    P3PeerInfo(
            UInt type,
            const UUIDPtr& pid,
            const UUIDPtr& fid,
            const EndpointPtr& m_cellEndpoint,
            const SAPInfoPtr& meshSAP,
            const SAPInfoPtr& discoverySAP,
            const SAPInfoPtr& ftSAP);
    virtual ~P3PeerInfo();

    bool isSensor();
    bool isSuperpeer();
    bool isCoordinator();
    bool isMonitor();

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    EndpointPtr& getCellEndpoint();
    
protected:
    EndpointPtr m_cellEndpoint;    
};


typedef ACE_Strong_Bound_Ptr<P3PeerInfo, ACE_SYNCH_MUTEX> P3PeerInfoPtr;

#endif	/* TREEPEERINFO_H */

