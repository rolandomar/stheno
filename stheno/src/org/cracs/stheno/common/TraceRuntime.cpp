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
 * File:   TraceRuntime.cpp
 * Author: rmartins
 * 
 * Created on February 15, 2011, 1:44 PM
 */

#include "TraceRuntime.h"
#include <ace/Time_Value.h>

TraceRuntime::TraceRuntime() :
m_closed(false), m_started(false), m_outputStream(0) {
}

TraceRuntime::~TraceRuntime() {
    close();
}

void TraceRuntime::close() {
    if (m_closed) {
        return;
    }
    m_closed = true;
    if (m_outputStream != 0) {
        delete m_outputStream;
        m_outputStream = 0;
    }
}

void TraceRuntime::open(String& filename) {
    if (m_started) {
        return;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    m_outputStream = new std::ofstream(m_filename.c_str());
}

void TraceRuntime::open(const char* filename) {
    if (m_started) {
        return;
    }
    m_closed = false;
    m_started = true;
    m_filename = filename;
    m_outputStream = new std::ofstream(filename);
}

ACE_OSTREAM_TYPE* TraceRuntime::getOutputStream() {
    return m_outputStream;
}

void TraceRuntime::logLine(String& line) {
    if (m_outputStream == 0) {
        return;
    }
    ACE_Time_Value now = ACE_OS::gettimeofday();
    (*m_outputStream) << now << " " << line << endl;
    m_outputStream->flush();
}

void TraceRuntime::logLine(std::string& line) {
    if (m_outputStream == 0) {
        return;
    }
    ACE_Time_Value now = ACE_OS::gettimeofday();
    (*m_outputStream) << now << " " << line << endl;
    m_outputStream->flush();
}

void TraceRuntime::logLine(std::string line) {
    if (m_outputStream == 0) {
        return;
    }
    ACE_Time_Value now = ACE_OS::gettimeofday();
    (*m_outputStream) << now << " " << line << endl;
    m_outputStream->flush();
}

void TraceRuntime::logRuntimeOpen(UUIDPtr& uuid) {
    stringstream ss;
    ss << "[RUNTIME_OPEN]"
            << " [UUID:" << uuid->toString().c_str() << "]";
    logLine(ss.str());
}

void TraceRuntime::logRuntimeClose(UUIDPtr& uuid) {
    stringstream ss;
    ss << "[RUNTIME_CLOSE]"
            << " [UUID:" << uuid->toString().c_str() << "]";
    logLine(ss.str());
}

void TraceRuntime::logBindToMesh(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& parentUUID, UUIDPtr& parentFID) {
    stringstream ss;
    ss << "[BIND]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [PUUID:" << parentUUID->toString().c_str() << "]"
            << " [PFID:" << parentFID->toString().c_str() << "]";

    logLine(ss.str());
}

void TraceRuntime::logMembershipTime(ACE_Time_Value& timeElapsed){
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec);
    printf("---------------------------- logMembershipTime %lu %lu\n",timeElapsed.msec(),usec);
    stringstream ss;
    ss << "[MEMBERSHIP_TIME]" << " [TIME:" << usec << "]";
    logLine(ss.str());
}

void TraceRuntime::logQueryTime(UInt type,ACE_Time_Value& timeElapsed){
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec);    
    //printf("---------------------------- logQueryTime %lu %lu\n",timeElapsed.msec(),usec);
    stringstream ss;
    ss << "[QUERY_TIME]" << " [TYPE:" << type << "] [TIME:" << usec << "]";
    logLine(ss.str());
}

void TraceRuntime::logMembershipRebindTime(ACE_Time_Value& timeElapsed){
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec);    
    printf("---------------------------- logMembershipRebindTime %lu %lu\n",timeElapsed.msec(),usec);
    stringstream ss;
    ss << "[MEMBERSHIP_REBIND_TIME]" << " [TIME:" << usec << "]";
    logLine(ss.str());
}

//no parent bind

void TraceRuntime::logBindToMesh(UUIDPtr& uuid, UUIDPtr& fid) {
    stringstream ss;
    ss << "[BIND]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]";

    logLine(ss.str());
}

void TraceRuntime::logUnbind(UUIDPtr& uuid) {
    stringstream ss;
    ss << "[UNBIND]"
            << " [UUID:" << uuid->toString().c_str() << "]";
    logLine(ss.str());
}

void TraceRuntime::logLocalServiceCreation(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& sid, UUIDPtr& iid) {
    stringstream ss;
    ss << "[LOCAL_CREATE_SERVICE]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]";
    logLine(ss.str());
}

void TraceRuntime::logLocalServiceCreationFT(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& sid, UUIDPtr& iid, ReplicationGroupPtr& rg) {
    stringstream ss;
    ss << "[LOCAL_CREATE_SERVICE_FT]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]"
            << " [PRIMARY:" << rg->getPrimary()->getUUID()->toString().c_str() << "]";
    list<OverlayPeerInfoPtr> lr = rg->getReplicaList();
    list<OverlayPeerInfoPtr>::iterator iter = lr.begin();
    while (iter != lr.end()) {
        ss << " [Replica:" << (*iter)->getUUID()->toString().c_str() << "]";
        iter++;
    }
    logLine(ss.str());
}

void TraceRuntime::logLocalServiceRemoval(UUIDPtr& uuid, UUIDPtr& fid, UUIDPtr& sid, UUIDPtr& iid) {
    stringstream ss;
    ss << "[LOCAL_REMOVE_SERVICE]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]";
    logLine(ss.str());
}


void TraceRuntime::logRemoteServiceCreation(const UUIDPtr& uuid, const UUIDPtr& fid, const UUIDPtr& sid, const UUIDPtr& iid, ACE_Time_Value& timeElapsed) {
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec); 
    stringstream ss;
    ss << "[REMOTE_CREATE_SERVICE]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]"
            << " [TIME:" << usec << "]";
    logLine(ss.str());
}

void TraceRuntime::logRemoteServiceCreationFT(const UUIDPtr& uuid, const UUIDPtr& fid, const UUIDPtr& sid, const UUIDPtr& iid, ACE_Time_Value& timeElapsed) {
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec); 
    stringstream ss;
    ss << "[REMOTE_CREATE_SERVICE_FT]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]"
            << " [TIME:" << usec << "]";
    logLine(ss.str());
}

void TraceRuntime::logRemoteServiceRemoval(const UUIDPtr& uuid, const UUIDPtr& fid, const UUIDPtr& sid, const UUIDPtr& iid,ACE_Time_Value& timeElapsed) {
    ACE_UINT64 usec;
    timeElapsed.to_usec(usec); 

    stringstream ss;
    ss << "[REMOTE_REMOVE_SERVICE]"
            << " [UUID:" << uuid->toString().c_str() << "]"
            << " [FID:" << fid->toString().c_str() << "]"
            << " [SID:" << sid->toString().c_str() << "]"
            << " [IID:" << iid->toString().c_str() << "]"
            << " [TIME:" << usec << "]";
    logLine(ss.str());
}
