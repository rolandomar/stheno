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
 * File:   PeerInfo.h
 * Author: rmartins
 *
 * Created on June 21, 2010, 7:08 PM
 */

#ifndef PEERINFO_H
#define	PEERINFO_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Time_Value.h>
#include <ace/ACE.h>
#include <ace/SString.h>
#include <ace/Synch.h>
#include <ace/Synch_Traits.h>
#include <ace/Bound_Ptr.h>
#include <ace/Recursive_Thread_Mutex.h>

#include <euryale/Types.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>
#include <stheno/service/exception/ServiceException.h>
#include <euryale/net/endpoint/SAPInfo.h>

class PeerInfo : public Serializable {
public:
    typedef ACE_Hash_Map_Manager<UUIDPtr, ServiceInfoPtr, ACE_SYNCH_RW_MUTEX> ServiceMap;

    PeerInfo(InputStream& inputStream);
    PeerInfo(const PeerInfo& p);
    PeerInfo(UInt type,
            const UUIDPtr& uuidPtr,
            const UUIDPtr& fidPtr,
            const SAPInfoPtr& meshSAP,
            const SAPInfoPtr& discoverySAP,
            const SAPInfoPtr& ftSAP);
    virtual ~PeerInfo();

    const UUIDPtr& getPID();
    const UUIDPtr& getFID();
    virtual UInt getType();

    //user services
    bool isServiceRunning(const UUIDPtr& sid);
    bool isServiceInstanceRunning(const UUIDPtr& sid, const UUIDPtr& iid);
    bool addService(const ServiceInfoPtr& service);
    bool addService(const UUIDPtr& sid);
    bool removeService(const UUIDPtr& sid);
    bool removeService(const UUIDPtr& sid, const UUIDPtr& iid);
    bool addServiceInstance(const UUIDPtr& sid,
            const UUIDPtr& iid,
            SAPInfoPtr& info);
    //list<EndpointPtr>& list);
    bool addServiceInstance(const UUIDPtr& sid,
            const UUIDPtr& iid);
    bool getInstanceOfService(const UUIDPtr& sid,
            const UUIDPtr& iid,
            ServiceInstanceInfoPtr& instance)
    throw (ServiceException&);
    void getInstancesOfService(const UUIDPtr& sid,
            list<UUIDPtr>& l)
    throw (ServiceException&);

    bool changeServiceIID(const UUIDPtr& sid, const UUIDPtr& iid, UUIDPtr& newIdd) throw (ServiceException&);

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);

    String& toString();

    UInt getTotalServicesRunning();

    SAPInfoPtr& getDiscoverySAP();
    SAPInfoPtr& getMeshSAP();
    SAPInfoPtr& getFTSAP();

protected:
    UInt m_type;
    UUIDPtr m_uuidPtr;
    UUIDPtr m_fidPtr;
    //service map
    //ACE_Hash_Map_Manager<UUIDPtr, ServiceInfoPtr, ACE_SYNCH_RW_MUTEX> m_serviceMap;
    ServiceMap m_serviceMap;
    //string rep
    String m_string;

    SAPInfoPtr m_meshSAP;
    SAPInfoPtr m_discoverySAP;
    SAPInfoPtr m_ftSAP;

    PeerInfo();

};
typedef ACE_Strong_Bound_Ptr<PeerInfo, ACE_SYNCH_MUTEX> PeerInfoPtr;

#endif	/* PEERINFO_H */

