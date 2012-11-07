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
 * File:   PeerQuery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 13, 2010, 2:30 PM
 */

#include "ServiceQuery.h"
#include "ServiceDiscoveryProvider.h"

const UInt ServiceQuery::SERVICE_QUERY =   0x34FACB32;

ServiceQuery::ServiceQuery(UUIDPtr& sid) : DiscoveryQuery(ServiceDiscoveryProvider::SERVICE_PROVIDER_TYPE,
        SERVICE_QUERY),m_sid(sid){
    
}

ServiceQuery::ServiceQuery(const ServiceQuery& orig):DiscoveryQuery(orig.m_provider,orig.m_type) {
    m_sid = orig.m_sid;
}

ServiceQuery::~ServiceQuery() {    
}

ServiceQuery::ServiceQuery(DiscoveryQuery* query):DiscoveryQuery(query)
{
    this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
}