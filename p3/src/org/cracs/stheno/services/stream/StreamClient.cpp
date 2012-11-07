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
 * File:   StreamClient.cpp
 * Author: rmartins
 * 
 * Created on November 22, 2010, 11:05 PM
 */

#include "StreamClient.h"
#include "stheno/Stheno.h"
#include "stheno/core/p2p/OverlayInterface.h"
#include "stheno/core/p2p/discovery/general_provider/FindServiceInstanceQueryReply.h"
#include <euryale/common/sleep/SleepHelper.h>
#include <stheno/core/p2p/discovery/general_provider/FindServiceInstanceQuery.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <stheno/services/stream/net/StreamServiceSAP.h>
#include <stheno/core/p2p/p3/P3Config.h>

StreamClient::StreamClient(Stheno* runtime)
: ServiceClient(runtime), m_client(0), m_frameCount(0), m_rebindStart(0), m_rebindTotalTime(0),
m_counter(0), m_totalLatency(0),m_closed(false) {
}

StreamClient::~StreamClient() {
}

int StreamClient::open(const UUIDPtr& sid, const UUIDPtr& iid, ClientParamsPtr& cp, UInt lastFrameNo) {
    if(m_closed){
        return -1;
    }
    if (m_sid.null()) {
        m_sid = sid;
    }
    if (m_iid.null()) {
        m_iid = iid;
    }
    if (m_cp.null()) {
        m_cp = cp;
    }


    FindServiceInstanceQuery* query = new FindServiceInstanceQuery(sid, iid);
    DiscoveryQueryReply* queryReply_ = this->getRuntime()->getOverlayInterface()->getDiscovery()->executeQuery(query);
    printf("\n\n\n\n\n********************StreamClient::open %s %s %p\n",
            sid->toString().c_str(),
            iid->toString().c_str(),
            queryReply_);
    if (queryReply_ == 0) {
        return -1;
    }
    if (queryReply_->isException()) {
        printf("StreamClient: exception! SID(%s) IID(%s)\n", sid->toString().c_str(), iid->toString().c_str());
        delete queryReply_;
        return -1;
    }
    FindServiceInstanceQueryReply *queryReply = new FindServiceInstanceQueryReply(queryReply_);

    //printf("StreamClient: FindServiceInstanceQueryReply!\n",queryReply->
    //EndpointPtr endpoint = m_streamParams->getEndpoint();
    Endpoint endpoint;
    queryReply->getSAPInfo()->getFirstEndpoint(endpoint);
    printf("StreamClient: FindServiceInstanceQueryReply! endpoint=%s\n", endpoint.toString().c_str());
    ACE_Connector<StreamServiceClientHandler, ACE_SOCK_Connector> connector;
    //QoSEndpoint qosE = *(endpoint.getQoS());        

    ThreadPerConnection *tpc = new ThreadPerConnection();
    //ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
    //        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
    ExecutionModelPtr* tpcPrt = new
            ExecutionModelPtr(tpc);
    UUIDPtr runtimeUUID;
    m_runtime->getUUID(runtimeUUID);
    UUIDPtr fid;
    m_runtime->getOverlayInterface()->getMesh()->getFID(fid);
    if (lastFrameNo == 0) {
        m_client = new StreamServiceClientHandler(
                runtimeUUID,
                fid,
                false, false, tpcPrt, 0, 0, 0);
    } else {
        m_client = new StreamServiceClientHandler(
                runtimeUUID,
                fid,
                lastFrameNo,
                false, false, tpcPrt, 0, 0, 0);
    }
    printf("StreamClient: before tpc\n");
    CPUReservation* reserve = 0;
    //CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_FIFO, CPUQoS::MAX_RT_PRIO);
    CPUQoS* cpuQoS = m_cp->createCPUQoS(); //new CPUPriorityQoS(m_cp->getScheduleType(), m_cp->getPriority());    
    tpc->bind(m_client);
    tpc->open(reserve, cpuQoS);
    //tpc->open();
    //tpc->bind(m_client);
    connector.reactor(tpc->getResources()->getReactor());
    printf("StreamClient: before connect\n");
    if (connector.connect(m_client, endpoint.getAddr()) == -1) {
        ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                ACE_TEXT("(%T)ERROR: SuperPeerDiscovery::executeQuery() - connect failed:")));
        printf("********************StreamClient:: connection failed\n");
        m_client->close();
        delete m_client;
        m_client = 0;
        return -1;
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamService::open_i() - Connect to service! %@\n"), m_client));

    }
    printf("StreamClient: after connect lastframe=%d\n", lastFrameNo);
    //m_client->setCloseListener(this);
    if (lastFrameNo != 0) {
        //It'a a reconnect!!!
        list<StreamFramePacket*>::iterator iter = m_client->getInitFrameList().begin();
        printf("StreamClient::open() - LastFrame=%d FRAMECOUNT= (%lu)\n", lastFrameNo, m_client->getInitFrameList().size());
        while (iter != m_client->getInitFrameList().end()) {
            if ((*iter)->getFrameNo() <= m_lastFrameNo) {
                printf("StreamClient::open() - ALREADY HAVE frame (%d)\n", (*iter)->getFrameNo());
            } else {
                if ((*iter)->getFrameNo() == (m_lastFrameNo + 1)) {
                    printf("StreamClient::open() - Good frame (%d)\n", (*iter)->getFrameNo());
                    m_lastFrameNo++;
                    m_frameCount++;
                } else {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("StreamClient::open() - Gap? frame (%d)\n"), (*iter)->getFrameNo()));
                    m_frameCount++;
                }
            }
            iter++;
        }
    } else {
        if (m_client->setCloseListener(this) == -1) {
            printf("StreamClient::open() - failed to setCloselistener\n");
            m_client->close();
            delete m_client;
            m_client = 0;
            return -1;
        }
    }
    m_client->setFrameListener(this);
    printf("StreamClient::open() - after set listener\n");
    if (m_client->asynchronous(true, false) != 0) {
        printf("+++++++++++++++++++++StreamClient::open() - failed to setasync\n");
        m_client->close();
        return -1;
    }
    if (m_rebindStart.msec() != 0) {
        ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - m_rebindStart;
        m_rebindTotalTime += rebindEnd;
    }
    printf("StreamClient::open() - ok\n");
    return 0;

}
//client shutdown/close

void StreamClient::close() {
    printf("StreamClient::close()\n");
    m_closed = true;
    if (m_client != 0) {
        m_client->close();
    }
}
//has to have the same SID of the service it belongs

const UUIDPtr& StreamClient::getSID() {
    return m_sid;
}

void StreamClient::onClose(AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>* channel) {
    ACE_Time_Value start = ACE_OS::gettimeofday();
    printf("********************StreamClient::onClose\n");
    if (channel != m_client) {
        printf("********************StreamClient::onClose WHAT====»»????\n");
    }
    channel->setCloseListener(0);
    channel->close();
    delete channel;
    m_client = 0;
    ACE_Time_Value rebindStart = ACE_OS::gettimeofday();
    //SleepHelper::sleep(3000);
    //SleepHelper::sleep(250);
    while (!m_closed) {
        while (open(m_sid, m_iid, m_cp, m_lastFrameNo) == -1) {
            printf("********************StreamClient::onClose still binding - m_client=%p\n", m_client);
            //SleepHelper::sleep(250);
        }
        if (m_client->setCloseListener(this) == 0) {
            break;
        } else {
            printf("********************StreamClient::onClose: fail to setcloselistener\n");
            m_client->close();
            delete m_client;
            m_client = 0;
        }
    }
    ACE_Time_Value rebindEnd = ACE_OS::gettimeofday() - rebindStart;
    m_rebindTotalTime += rebindEnd;

    printf("********************StreamClient::onClose - FNISHED!\n");
}

void StreamClient::onFrame(StreamFramePacket* frame) {
    if (m_lastFrameNo == 0) {
        m_lastFrameNo = frame->getFrameNo();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClient::handlePacket() - First frame!(%d)\n"), frame->getFrameNo()));
        ACE_Time_Value delta = ACE_OS::gettimeofday();
        delta -= frame->getTimestamp();
        m_counter++;
        m_totalLatency += delta;
    } else {
        if ((m_lastFrameNo + 1) == frame->getFrameNo()) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClient::handlePacket() - new frame!(%d) current=%d\n"), 
            //        frame->getFrameNo(),m_lastFrameNo));
            m_lastFrameNo++;
            m_frameCount++;
            ACE_Time_Value delta = ACE_OS::gettimeofday();
            delta -= frame->getTimestamp();
            m_counter++;
            m_totalLatency += delta;
        } else {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: StreamClient::handlePacket() - OUT OF SEQUENCE (%d) current=%d?\n"),
                    frame->getFrameNo(), m_lastFrameNo));
            m_lastFrameNo = frame->getFrameNo();
            m_frameCount++;
        }
    }
    delete frame;
}
