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
 * File:   PeerQueryReply.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 13, 2010, 2:40 PM
 */

#include "ServiceQueryReply.h"
#include "ServiceDiscoveryProvider.h"

ServiceQueryReply::ServiceQueryReply():DiscoveryQueryReply(ServiceDiscoveryProvider::SERVICE_PROVIDER_TYPE,
        SERVICE_QUERY_REPLY){

}

ServiceQueryReply::ServiceQueryReply(list<ServiceInstanceInfoPtr>& instanceList):
DiscoveryQueryReply(ServiceDiscoveryProvider::SERVICE_PROVIDER_TYPE,
        SERVICE_QUERY_REPLY)
{
    this->add(instanceList);
}

ServiceQueryReply::ServiceQueryReply(DiscoveryException* ex):
DiscoveryQueryReply(ServiceDiscoveryProvider::SERVICE_PROVIDER_TYPE,
        SERVICE_QUERY_REPLY,ex)
{    
}

ServiceQueryReply::ServiceQueryReply(DiscoveryQueryReply* query):DiscoveryQueryReply(query)
{
    if(m_ex == 0){
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

ServiceQueryReply::~ServiceQueryReply() {
    
}
