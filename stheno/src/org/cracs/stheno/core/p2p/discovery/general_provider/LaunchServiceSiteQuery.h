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
 * File:   LaunchServiceSiteQuery.h
 * Author: rmartins
 *
 * Created on November 15, 2010, 5:45 PM
 */

#ifndef LAUNCHSERVICESITEQUERY_H
#define	LAUNCHSERVICESITEQUERY_H

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/common/io/ByteInputStream.h>

class LaunchServiceSiteQuery : public DiscoveryQuery {
public:

    //static const int LAUNCHSERVICESITE_QUERY = 0xA823FB00;
    static const int LAUNCHSERVICESITE_QUERY = 0x00FF;
    
    LaunchServiceSiteQuery(const UUIDPtr& requester,const UUIDPtr& sid,list<UUIDPtr>& filterPeers);
    LaunchServiceSiteQuery(DiscoveryQuery* query);
    LaunchServiceSiteQuery(const LaunchServiceSiteQuery& orig);
    virtual ~LaunchServiceSiteQuery();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
    
    UUIDPtr& getSID();
    UUIDPtr& getRequester();
    list<UUIDPtr>& getFilterPeers();
    
protected:
    list<UUIDPtr> m_filterPeers;
    UUIDPtr m_sid;
    UUIDPtr m_requester;
    
    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);    
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);   
};

#endif	/* LAUNCHSERVICESITEQUERY_H */

