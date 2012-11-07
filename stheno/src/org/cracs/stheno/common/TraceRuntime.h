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
 * File:   TraceRuntime.h
 * Author: rmartins
 *
 * Created on February 15, 2011, 1:44 PM
 */

#ifndef TRACERUNTIME_H
#define	TRACERUNTIME_H

#include <euryale/Types.h>
#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#include <euryale/common/uuid/UUID.h>
#include <sstream> 
#include <string>
using std::stringstream;
using std::string;

#include <stheno/core/p2p/ft/ReplicationGroup.h>


class TraceRuntime {
public:
    TraceRuntime();    
    virtual ~TraceRuntime();
    
    //open
    void open(String& filename);
    void open(const char* filename);
    
    //Aux.
    ACE_OSTREAM_TYPE* getOutputStream();
    void logRuntimeOpen(UUIDPtr& uuid);
    void logRuntimeClose(UUIDPtr& uuid);
    //with parent bind
    void logBindToMesh(UUIDPtr& uuid, UUIDPtr& fid,UUIDPtr& parentUUID,UUIDPtr& parentFID);
    //no parent bind
    void logBindToMesh(UUIDPtr& uuid, UUIDPtr& fid);
    void logUnbind(UUIDPtr& uuid);
    void logMembershipTime(ACE_Time_Value& timeElapsed);
    void logMembershipRebindTime(ACE_Time_Value& timeElapsed);
    void logQueryTime(UInt type,ACE_Time_Value& timeElapsed);
    
    
    void logLocalServiceCreation(UUIDPtr& uuid,UUIDPtr& fid,UUIDPtr& sid,UUIDPtr& iid);
    void logLocalServiceCreationFT(UUIDPtr& uuid,UUIDPtr& fid,UUIDPtr& sid,UUIDPtr& iid,ReplicationGroupPtr& rg);
    void logLocalServiceRemoval(UUIDPtr& uuid,UUIDPtr& fid,UUIDPtr& sid,UUIDPtr& iid);
    
    void logRemoteServiceCreation(const UUIDPtr& uuid, const UUIDPtr& fid, const UUIDPtr& sid, const UUIDPtr& iid, ACE_Time_Value& timeElapsed);
    void logRemoteServiceCreationFT(const UUIDPtr& uuid,const UUIDPtr& fid,const UUIDPtr& sid,const UUIDPtr& iid,ACE_Time_Value& timeElapsed);
    void logRemoteServiceRemoval(const UUIDPtr& uuid,const UUIDPtr& fid,const UUIDPtr& sid,const UUIDPtr& iid,ACE_Time_Value& timeElapsed);
    
    //log a line
    void logLine(String& line);
    void logLine(std::string& line);
    void logLine(std::string line);
    
    void close();
protected:
    String m_filename;
    bool m_started;
    bool m_closed;
    ACE_OSTREAM_TYPE *m_outputStream;

};

typedef ACE_Singleton<TraceRuntime, ACE_Thread_Mutex>  TraceRuntimeSingleton;

#endif	/* TRACERUNTIME_H */

