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
 * File:   LaunchServiceSiteQueryReply.h
 * Author: rmartins
 *
 * Created on November 15, 2010, 5:45 PM
 */

#ifndef LAUNCHSERVICESITEQUERYREPLY_H
#define	LAUNCHSERVICESITEQUERYREPLY_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

class LaunchServiceSiteQueryReply: public DiscoveryQueryReply {
public:
    
    static const UInt LAUNCHSERVICESITE_QUERY_REPLY = 0x01123AF;
    
    LaunchServiceSiteQueryReply(UUIDPtr& uuid,UUIDPtr& fid,SAPInfoPtr& meshSAP,SAPInfoPtr& ftSAP);
    LaunchServiceSiteQueryReply(DiscoveryQueryReply* query);
    virtual ~LaunchServiceSiteQueryReply();
   

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);    
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
    
    UUIDPtr& getUUID();    
    UUIDPtr& getFID();    
    SAPInfoPtr& getMeshSAPInfo();    
    SAPInfoPtr& getFTSAPInfo();

protected:
    UUIDPtr m_uuid;
    UUIDPtr m_fid;
    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_ftSAP;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
   
};


#endif	/* LAUNCHSERVICESITEQUERYREPLY_H */

