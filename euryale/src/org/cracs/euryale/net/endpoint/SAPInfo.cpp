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
 * File:   SAP.cpp
 * Author: rmartins
 * 
 * Created on August 18, 2010, 3:46 PM
 */

#include <euryale/net/endpoint/SAPInfo.h>

SAPInfo::SAPInfo() {
}

SAPInfo::SAPInfo(const SAPInfo& orig) : m_endpointList(orig.m_endpointList) {
}

SAPInfo::~SAPInfo() {
}

SAPInfo::SAPInfo(InputStream& inputStream){
    deserialize(inputStream);
}

bool SAPInfo::addEndpoint(Endpoint& endpoint) {
    list<Endpoint>::iterator iter = m_endpointList.begin();
    while (iter != m_endpointList.end()) {
        if ((*iter).isEqual(endpoint)) {
            return false;
        }
        iter++;
    }
    m_endpointList.push_back(endpoint);
    return true;
}

bool SAPInfo::addEndpointList(list<EndpointPtr>& endpointList){
    list<EndpointPtr>::iterator iter = endpointList.begin();
    while (iter != endpointList.end()) {
        list<Endpoint>::iterator _iter = m_endpointList.begin();
        while (_iter != m_endpointList.end()) {
            if ((*_iter).isEqual(*(*iter))) {
                return false;
            }
            _iter++;
        }
        iter++;
    }

    iter = endpointList.begin();
    while (iter != endpointList.end()) {
        addEndpoint(*(*iter));
        iter++;
    }
    return true;
}

bool SAPInfo::addEndpointList(list<Endpoint>& endpointList) {
    list<Endpoint>::iterator iter = endpointList.begin();
    while (iter != endpointList.end()) {
        list<Endpoint>::iterator _iter = m_endpointList.begin();
        while (_iter != m_endpointList.end()) {
            if ((*_iter).isEqual((*iter))) {
                return false;
            }
            _iter++;
        }
        iter++;
    }

    iter = endpointList.begin();
    while (iter != endpointList.end()) {
        addEndpoint(*iter);
        iter++;
    }
    return true;

}

bool SAPInfo::removeEndpoint(Endpoint& endpoint) {
    list<Endpoint>::iterator iter = m_endpointList.begin();
    while (iter != m_endpointList.end()) {
        if ((*iter).isEqual(endpoint)) {
            m_endpointList.erase(iter);
            return true;
        }
        iter++;
    }
    return false;
}

void SAPInfo::getEndpointList(list<Endpoint>& el) {
    el = m_endpointList;
}

void SAPInfo::getEndpoint(const QoSEndpoint& qos,Endpoint& el) throw (EndpointException&) {
    list<Endpoint>::iterator iter = m_endpointList.begin();
    while (iter != m_endpointList.end()) {
        printf("QoSEndpoint(%d,%d)\n",(*iter).getQoS()->getType(),(*iter).getQoS()->getPriority());
        if ((*iter).getQoS()->isEqual(qos)) {
            //return *iter;
            el = *iter;
            return;
        }
        iter++;
    }
    throw EndpointException("Endpoint QoS not available");
}

void SAPInfo::getFirstEndpoint(Endpoint& e) const throw (EndpointException&){
    if (m_endpointList.size() == 0) {
        throw EndpointException("No endpoints available");
    }
    e = m_endpointList.front();
}

void SAPInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    UInt size = m_endpointList.size();
    outputStream.write_ulong(size);
    list<Endpoint>::iterator iter = m_endpointList.begin();
    while (iter != m_endpointList.end()) {
        (*iter).serialize(outputStream);
        iter++;
    }
}

void SAPInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    //clear any cached data
    m_endpointList.clear();
    UInt size = 0;
    inputStream.read_ulong(size);
    for(int i=0; i < size; i++){
        Endpoint e(inputStream);
        addEndpoint(e);
    }
}
