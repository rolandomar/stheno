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
 * File:   Runtime.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 10, 2010, 1:00 PM
 */

#include "ServiceManagement.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ServiceManagement);

#include <stheno/Stheno.h>
#include <stdio.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <stheno/core/p2p/common/PeerMap.h>
#include <stheno/core/p2p/common/ServiceInfo.h>
#include <euryale/common/ListHelper.h>

class PeerInfoEx: public PeerInfo{
public:
    PeerInfoEx(const UUIDPtr& uuidPtr,const UUIDPtr& fidPtr,const SAPInfoPtr& meshSAP,
            const SAPInfoPtr& discoverySAP,
            const SAPInfoPtr& ftSAP):PeerInfo(0,uuidPtr,fidPtr,meshSAP,discoverySAP,ftSAP){}
    virtual ~PeerInfoEx(){}       
};



ServiceManagement::ServiceManagement() {
}

ServiceManagement::~ServiceManagement() {
}

void ServiceManagement::setUp() {
    
}

void ServiceManagement::tearDown() {
    
}

void ServiceManagement::testMethod() {
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\n")));
    PeerMap map;
    UUIDPtr pid(UUID::generateUUID());
    UUIDPtr pid2(UUID::generateUUID());
    UUIDPtr fid(UUID::generateUUID());
    UUIDPtr sid(UUID::generateUUID());
    UUIDPtr iid1(UUID::generateUUID());
    UUIDPtr iid2(UUID::generateUUID());
    
    SAPInfoPtr meshSAP;
    SAPInfoPtr ftSAP;
    SAPInfoPtr discoverySAP;
    
    
    PeerInfoPtr peerPtr(new PeerInfoEx(pid,fid,meshSAP,discoverySAP,ftSAP));

    map.addPeer(peerPtr);

    try{
        PeerInfoPtr pi;
        map.getPeerInfo(pid,pi);
    }catch(PeerException& ex){
      ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ex=%s\n"),ex.toString().c_str()));
    }

    try{
        PeerInfoPtr pi;
        map.getPeerInfo(pid2,pi);
    }catch(PeerException& ex){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ex2=%s\n"),ex.toString().c_str()));
    }



    peerPtr->addServiceInstance(sid,iid1);
    
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Output:\n%s\n"),peerPtr->toString().c_str()));

    list<UUIDPtr> l;
    peerPtr->getInstancesOfService(sid,l);    
    ListHelper<UUIDPtr>::printSmartPtrList(l);
    
    ServiceInstanceInfoPtr i;
    peerPtr->getInstanceOfService(sid,l.front(),i);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)=====%s\n"),i->toString().c_str()));

    //bool ret = peerPtr->removeService(sid);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)after remove = %d\n",ret);
    //delete l;
    CPPUNIT_ASSERT(true);
}
