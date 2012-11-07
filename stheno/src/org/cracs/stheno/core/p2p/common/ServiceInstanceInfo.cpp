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
 * File:   ServiceInstanceInfo.cpp
 * Author: rmartins
 * 
 * Created on June 21, 2010, 11:49 AM
 */

#include "ServiceInstanceInfo.h"
#include "ServiceInfo.h"

ServiceInstanceInfo::ServiceInstanceInfo(const UUIDPtr& sid, const UUIDPtr& iid) :
m_sid(sid), m_iid(iid) {
}

ServiceInstanceInfo::ServiceInstanceInfo(const ServiceInstanceInfo& si) :
m_sid(si.m_sid), m_iid(si.m_iid), m_sapInfo(si.m_sapInfo) {

}

ServiceInstanceInfo::ServiceInstanceInfo(InputStream& inputStream) {
    deserialize(inputStream);
}

ServiceInstanceInfo::~ServiceInstanceInfo() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)~ServiceInstanceInfo(%@)\n"), this));
}

ServiceInstanceInfo::ServiceInstanceInfo(const UUIDPtr& sid, const UUIDPtr& iid, SAPInfoPtr& sapInfo) :
m_sid(sid), m_iid(iid), m_sapInfo(sapInfo) {

}

String& ServiceInstanceInfo::toString() {    
    stringstream ss;
    //m_string = "ServiceInstanceInfo(" + m_iid->toString() + ")\n";
    ss << "ServiceInstanceInfo(";
    
    if(!m_iid.null()){
     ss << m_iid->toString() << ")\n";
    }else{
        ss << ")";
    }
    /*list<EndpointPtr>::iterator iter = m_endpointList.begin();
    while(iter != m_endpointList.end()){
        m_string += "\t"+(*iter)->toString() +"\n";
        iter++;
    }*/
    m_string = ss.str().c_str();
    return m_string;
}

UUIDPtr& ServiceInstanceInfo::getIID() {
    return m_iid;
}

void ServiceInstanceInfo::setIID(UUIDPtr& iid) {
    m_iid = iid;
}

UUIDPtr& ServiceInstanceInfo::getSID() {
    return m_sid;
}

SAPInfoPtr& ServiceInstanceInfo::getSAPInfo() {
    return m_sapInfo;
}

void ServiceInstanceInfo::serialize(OutputStream& outputStream) throw (SerializationException&) {
    m_sid->serialize(outputStream);
    m_iid->serialize(outputStream);
    if (!m_sapInfo.null()) {
        outputStream.write_boolean(true);
        m_sapInfo->serialize(outputStream);
    } else {
        outputStream.write_boolean(false);
    }
}

void ServiceInstanceInfo::deserialize(InputStream& inputStream) throw (SerializationException&) {
    UUID* sid = new UUID(inputStream);
    m_sid.reset(sid);
    UUID* iid = new UUID(inputStream);
    m_iid.reset(iid);
    Boolean flag;
    inputStream.read_boolean(flag);
    if (flag) {
        SAPInfo* info = new SAPInfo(inputStream);
        m_sapInfo.reset(info);
    } else {
        m_sapInfo.reset(0);
    }
}
