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
 * File:   LeafDiscovery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 6, 2010, 11:20 AM
 */

#include <stheno/core/p2p/p3/leaf/mesh/LeafMeshDiscovery.h>
#include <stheno/core/p2p/p3/superpeer/cell/net/CellAddr.h>

#include <euryale/common/sleep/IncrementalSleep.h>

LeafMeshDiscovery::LeafMeshDiscovery(UUIDPtr& uuid) : /*m_tpc(0),*/ m_uuid(uuid),
m_channel(0) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafMeshDiscovery::LeafMeshDiscovery(%@)\n"),this));
}

LeafMeshDiscovery::~LeafMeshDiscovery() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafMeshDiscovery::~LeafMeshDiscovery(%@)\n"),this));
    close();    

}

void LeafMeshDiscovery::close() {
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_channel != 0) {
        m_channel->close();
        delete m_channel;
        m_channel = 0;
    }
    /*m_tpc->shutdown();
    delete m_tpc;
    m_tpc = 0;*/

}

void LeafMeshDiscovery::open() throw (CellDiscoveryException&) {
    m_channel = new CellDiscoveryChannel(0, false, false, false);
    String itf;
    String ip;
    Short port = 0;
    try {

        if (!P3Configuration::instance()->lookupValue("CELL_MULTICAST_INTERFACE", itf)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        if (!P3Configuration::instance()->lookupValue("CELL_ROOT_DISCOVERY_IP", ip)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        if (!P3Configuration::instance()->lookupValue("CELL_ROOT_DISCOVERY_PORT", port)) {
            throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::open() - addr(%s,%s,%d)\n"), itf.c_str(), ip.c_str(), port));
    } catch (libconfig::FileIOException& ex) {
        throw CellException(CellException::MISSING_VALUE_FROM_PPROPERTIES);
    }
    //EM setup
    //m_tpc = new ThreadPerConnection();
    //open the EM
    //m_tpc->open();
    //end of EM setup
    //binds the channel to the ec, sets the reactor
    //m_tpc->bind(m_channel);
    //opens the channel with the proper settings
    //"lo"
    ACE_INET_Addr addr(port, ip.c_str());
    if (m_channel->open(addr, itf.c_str(), 1) == -1) {
        close();
        throw CellException(CellException::FAILED_TO_BIND_ADDR);
    }
}

int LeafMeshDiscovery::sendPacket(SthenoPacket* packet, ACE_Time_Value* timeout) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,-1);
    if(m_channel == 0){
        return -1;
    }
    return m_channel->send(packet, timeout);

}

/*void LeafDiscovery::asynchronous() {
    m_channel->asynchronous(true, true);
}*/

/*void LeafDiscovery::handleDiscoveryPacket(SthenoPacket* packet) {
    //TODO: match to request, otherwise delete it
    delete packet;
}*/



CoordinatorInfo* LeafMeshDiscovery::requestCoordinator() throw (CellDiscoveryException&) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock,0);
    if (m_channel == 0) {
        return 0;
    }
   
    int timeoutPerTry = REQUEST_CELL_TIMEOUT_MS;
    ACE_Time_Value overallTimeout = ACE_OS::gettimeofday();
    ACE_Time_Value overallTimeoutDelta;
    overallTimeoutDelta.msec(MAX_WAIT_TIME);//REQUEST_CELL_TIMEOUT_MS*20);
    overallTimeout += overallTimeoutDelta;    
    //IncrementalSleep sleeper(0,50000);
    int requestIntervalTimeout = REQUEST_CELL_TIMEOUT_INTERVAL_MS;
    while (ACE_OS::gettimeofday() < overallTimeout) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafMeshDiscovery::requestCoordinator() - Inside loop\n")));
        ACE_Time_Value* sendTimeout = 0;
        RequestCoordinatorPacket *packet = new RequestCoordinatorPacket(m_uuid,
                CellID::INVALID_CELL_ID_UUIDPTR,
                m_uuid,
                CellID::INVALID_CELL_ID_UUIDPTR,
                0);
        m_channel->send(packet, sendTimeout);        
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafMeshDiscovery::requestCoordinator() - after send\n")));
        delete packet;        
        packet = 0;
        
        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(timeoutPerTry);
        timeout += delta;

        while (ACE_OS::gettimeofday() < timeout) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)recv\n");            
            ACE_Time_Value receiveTimeout = timeout - ACE_OS::gettimeofday();
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)now=%Q timeout=%Q\n"), receiveTimeout.msec(), timeout.msec()));
            SthenoPacket* recvPacket = 0;
            m_channel->receivePacket(recvPacket, &receiveTimeout);
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)after recv\n");
            if (recvPacket != 0) {
                if (recvPacket->getPacketHeader()->getSrcPID()->compare(*(m_uuid.get())) == 0) {
                    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)GOT PACKERT but its our own:%d\n"), recvPacket->getPacketHeader()->getType()));
                    delete recvPacket;                    
                    //recvPacket = 0;                    
                    continue;
                } else {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafDiscovery::requestCoordinator() - GOT PACKERT and it's not our own:%d\n"),
                    //        recvPacket->getPacketHeader()->getType()));
                    switch (recvPacket->getPacketHeader()->getType()) {
                        case REQUEST_COORDINATOR_REPLY_PACKET:
                        {
                            RequestCoordinatorReplyPacket* replyPacket =
                                    static_cast<RequestCoordinatorReplyPacket*> (recvPacket);
                            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafDiscovery::requestCoordinator() - uuid(%s)\n"),
                                   // replyPacket->getPID()->toString().c_str()
                                   // ));                            
                            CoordinatorInfo* info = new CoordinatorInfo(replyPacket->getPID(), replyPacket->getCellID(),
                                    replyPacket->getMeshSAP(), replyPacket->getDiscoverySAP(), replyPacket->getFTSAP(),
                                    replyPacket->getCoordinatorEndpoint());                                                        
                            delete replyPacket;                            
                            return info;
                        }
                        default:
                        {
                            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: LeafDiscovery:: packet not known\n")));
                            delete recvPacket;                                               
                            break;
                        }
                    }
                }
            }
        }
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) LeafMeshDiscovery::requestCoordinator(): timeout\n")));   
        SleepHelper::sleepRandom(REQUEST_CELL_TIMEOUT_INTERVAL_MS);        
        //SleepHelper::sleepRandom(requestIntervalTimeout);        
        //requestIntervalTimeout *= requestIntervalTimeout * 15;
        //sleeper.sleep();
    }
    return 0;
}
