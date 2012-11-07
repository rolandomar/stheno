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
 * File:   RoutingTableTest.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 19, 2010, 12:31:04 PM
 */

#include "RoutingTableTest.h"
#include <euryale/common/ListHelper.h>
#include <stheno/core/p2p/routing/RoutingTable2.h>
#include <stheno/core/p2p/common/PeerInfo.h>

CPPUNIT_TEST_SUITE_REGISTRATION(RoutingTableTest);

RoutingTableTest::RoutingTableTest() {
}

RoutingTableTest::~RoutingTableTest() {
}

void RoutingTableTest::setUp() {
}

void RoutingTableTest::tearDown() {
}

void RoutingTableTest::testMethod() {
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)\nRoutingTableTest\n");
    UUIDPtr* uuid = UUID::generateUUID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)uuid=%s\n",(*uuid)->toString().c_str());
    UUIDPtr* fid = UUID::generateUUID();
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)fid =%s\n",(*fid)->toString().c_str());
    PeerInfo peerInfo(*uuid,*fid);
    QoSEndpoint* qos = new QoSEndpoint(10);
    String* host = new String("localhost");
    UShort port = 1011;
    Endpoint *endpoint = new Endpoint(Endpoint::TCP,host,port,qos);
    UUIDPtr* sid = UUID::generateUUID();
    UUIDPtr* iid = UUID::generateUUID();
    peerInfo.insertEndpoint(*sid,*iid,endpoint);
    list<EndpointPtr*>* l = peerInfo.getEndpointList(*iid);
    cout << (*(l->front()))->toString() << endl;
    ListHelper<EndpointPtr*>::deleteList(l);
    /*RoutingTable2 table;
    UUID* uuid = UUID::generateUUID();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)uuid=%s\n",uuid->toString().c_str());
    UUID* fid = UUID::generateUUID();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)fid =%s\n",fid->toString().c_str());
    UUID* sid = UUID::generateUUID();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)sid =%s\n",sid->toString().c_str());
    QoSEndpoint* qos = new QoSEndpoint(10);
    String* host = new String("localhost");
    UShort port = 1011;
    Endpoint *endpoint = new Endpoint(Endpoint::TCP,host,port,qos);
    int ret = table.insert(*uuid,*fid,*sid,endpoint);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ret=%d\n",ret);
    int size = table.getEndpointList(*uuid,*fid,*sid)->get()->size();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)size=%d\n",size);
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)Endpoint=%s\n",table.getEndpointList(*uuid,*fid,*sid)->get()->front()->get()->toString().c_str());*/
    CPPUNIT_ASSERT(true);
}

