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
 * File:   DiscoveryQuery_test.cpp
 * Author: rmartins
 *
 * Created on Sep 21, 2010, 9:40:23 AM
 */

#include "DiscoveryQuery_test.h"
#include <stheno/core/p2p/discovery/peer_provider/PeerQuery.h>
#include <stheno/core/p2p/discovery/service_provider/ServiceQueryReply.h>
#include <euryale/common/io/ByteOutputStream.h>
#include <euryale/common/io/ByteInputStream.h>
#include <stheno/core/p2p/common/ServiceInstanceInfo.h>

#include <euryale/event/Event.h>
#include <euryale/timer/TimerManager.h>

CPPUNIT_TEST_SUITE_REGISTRATION(DiscoveryQuery_test);

DiscoveryQuery_test::DiscoveryQuery_test() {
}

DiscoveryQuery_test::~DiscoveryQuery_test() {
}

void DiscoveryQuery_test::setUp() {
}

void DiscoveryQuery_test::tearDown() {
}

void DiscoveryQuery_test::testMethod() {
    
    Event *ev = new Event();
    ACE_Timer_Heap *t = new ACE_Timer_Heap;
    ACE_Time_Value timeout = t->gettimeofday();
    //printf("T=%d \n", timeout.sec());
    ACE_Time_Value delta(2,0);
    timeout += delta;    
    /*long tid = t.schedule(ev,0,timeout);
    printf("tid=%lu %d\n",tid,t.is_empty());
    ACE_Time_Value rr;
    printf("T=%d %d\n", t.calculate_timeout(&rr)->sec(),rr.sec());
    
    sleep(11);
    t.expire();
    sleep(11);
    t.expire();
    sleep(100);
    */
    TimerManager tm(t);
    tm.open();
    tm.getTimerQueue()->schedule(ev,0,timeout);
    sleep(100);
    
    UUID* uuid = UUID::generateUUID();
    UUIDPtr uuidPtr(uuid);
    PeerQuery* query = new PeerQuery(uuidPtr);
    
    StreamSize ss;
    query->serialize(ss);
    ByteOutputStream os(ss.total_length());
    query->serialize(os);

    ACE_Message_Block* mb = new ACE_Message_Block(RDR::total_length(&os.start_, 0));
    RDR::consolidate(mb, &os.start_);
    //query->serialize(os);
    ByteInputStream inputStream(mb);
    DiscoveryQuery* q = new DiscoveryQuery(inputStream);
    PeerQuery* newQ = new PeerQuery(q);
    /*ACE_Message_Block* mb = new ACE_Message_Block(2048);
    ByteInputStream inputStream(mb);*/
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) Query: %s\n"), newQ->getUUID()->toString().c_str()));


    QoSEndpoint* qosE = new QoSEndpoint(Endpoint::TCP, 50);
    String host("127.0.0.1");
    Endpoint* e = new Endpoint(Endpoint::TCP, host, 8888, qosE);
    EndpointPtr ePtr(e);
    UUID* sid = UUID::generateUUID();
    UUID* iid = UUID::generateUUID();
    UUIDPtr iidPtr(iid);
    UUIDPtr sidPtr(sid);
    //list<EndpointPtr> le;
    //le.push_back(ePtr);
    SAPInfo* sapInfo = new SAPInfo();
    sapInfo->addEndpoint(*e);
    SAPInfoPtr sapInfoPtr(sapInfo);
    ServiceInstanceInfo* info = new ServiceInstanceInfo(sidPtr,iidPtr, sapInfoPtr);
    ServiceInstanceInfoPtr infoPtr(info);
    list<ServiceInstanceInfoPtr> lInfo;
    lInfo.push_back(infoPtr);
    ServiceQueryReply* sqr = new ServiceQueryReply(lInfo);
    StreamSize ss2;
    sqr->serialize(ss2);
    ByteOutputStream os2(ss2.total_length());
    sqr->serialize(os2);
    ACE_Message_Block* mb2 = new ACE_Message_Block(RDR::total_length(&os2.start_, 0));
    RDR::consolidate(mb2, &os2.start_);
    //query->serialize(os);
    ByteInputStream inputStream2(mb2);
    DiscoveryQueryReply* qr = new DiscoveryQueryReply(inputStream2);
    ServiceQueryReply* newQr = new ServiceQueryReply(qr);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T) QueryReply: %s\n"), newQr->getServiceInfo().front()->toString().c_str()));

    //CPPUNIT_ASSERT(true);

}

/*void DiscoveryQuery_test::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}*/

