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
 * File:   CellDiscovery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 26, 2010, 3:32 PM
 */

#include "CellDiscovery.h"
#include <stheno/core/p2p/p3/superpeer/cell/net/CellAddr.h>
#include <euryale/runtime/RuntimeConfiguration.h>
#include <stheno/core/QoSManagerInterface.h>

CellDiscovery::CellDiscovery(UUIDPtr& uuid, QoSManagerInterface* qosManager) :
m_cell(0), m_tpc(0), m_uuid(uuid), m_qosManager(qosManager), m_channel(0) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO CellDiscovery::CellDiscovery()\n")));
    m_debugCellDiscovery = RuntimeConfiguration::isClassDebugged("CellDiscovery");
}

CellDiscovery::~CellDiscovery() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO CellDiscovery::~CellDiscovery()\n")));
    close();
}

QoSManagerInterface* CellDiscovery::getQoSManager() {
    return m_qosManager;
}

void CellDiscovery::close() {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO CellDiscovery::close()\n")));
    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (m_channel != 0) {
        m_channel->close();
        delete m_channel;
        m_channel = 0;
    }
    if (m_tpc != 0) {
        m_tpc->shutdown();
        delete m_tpc;
        m_tpc = 0;
    }
    m_cell = 0;
    m_keepAliveCounter = 0;
}

int CellDiscovery::sendPacket(SthenoPacket* packet, ACE_Time_Value* timeout) {
    int ret = m_channel->send(packet, timeout);
    if (DOUBLE_SEND_KERNEL_2_6_39_BUG) {
        //if (ret == -1) {
        //    return -1;
        //}
        ret = m_channel->send(packet, timeout);
    }
    return ret;

}

Endpoint* CellDiscovery::findCoordinator() throw (CellDiscoveryException&) {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    FindCoordinatorPacket *packet = 0;
    //m_channel->synchronous();    
    int timeoutPerTry = REQUEST_CELL_TIMEOUT_MS;
    ACE_Time_Value overallTimeout = ACE_OS::gettimeofday();
    ACE_Time_Value overallTimeoutDelta;
    //overallTimeoutDelta.msec(REQUEST_CELL_TIMEOUT_MS);
    overallTimeoutDelta.msec(MAX_WAIT_TIME);
    overallTimeout += overallTimeoutDelta;
    //flag if we catch a coordinator peer
    //UUID* coordinatorPeer = 0;
    RequestCellPacket* cellRequestPacket = 0;
    while (ACE_OS::gettimeofday() < overallTimeout) {
        //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellDiscovery::findCoordinator() - Inside loop\n");
        ACE_Time_Value* sendTimeout = 0;
        packet = new FindCoordinatorPacket(m_uuid,
                CellID::INVALID_CELL_ID_UUIDPTR,
                UUID::INVALID_ID,
                CellID::INVALID_CELL_ID_UUIDPTR,
                //CellID::ROOT_CELL_ID_UUIDPTR,
                0);
        int ret = m_channel->send(packet, sendTimeout);
        if (DOUBLE_SEND_KERNEL_2_6_39_BUG) {
            /*if (ret == -1) {
                delete packet;
                return 0;
            }*/
            ret = m_channel->send(packet, sendTimeout);
        }
        delete packet;
        if (ret == -1) {
            return 0;
        }
        SthenoPacket* recvPacket = 0;

        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(timeoutPerTry);
        timeout += delta;

        while (ACE_OS::gettimeofday() < timeout) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)recv\n");
            //ACE_Time_Value receiveTimeout(0, 1000);
            ACE_Time_Value receiveTimeout = timeout - ACE_OS::gettimeofday();
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)now=%lu timeout=%lu\n", receiveTimeout.msec(), timeout.msec());
            recvPacket = 0;
            m_channel->receivePacket(recvPacket, &receiveTimeout);
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)after recv\n");
            if (recvPacket != 0) {
                if (recvPacket->getPacketHeader()->getSrcPID()->compare(*(m_uuid.get())) == 0) {
                    //if (m_debugCellDiscovery) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)GOT PACKERT but its our own:%d\n"), recvPacket->getPacketHeader()->getType()));
                    //}
                    delete recvPacket;
                    recvPacket = 0;
                    continue;
                } else {
                    if (m_debugCellDiscovery) {
                        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::findCoordinator() - GOT PACKERT and it's not our own:%d\n"),
                                recvPacket->getPacketHeader()->getType()));
                    }
                    switch (recvPacket->getPacketHeader()->getType()) {
                        case FIND_COORDINATOR_REPLY_PACKET:
                        {
                            CoordinatorReplyPacket* cellReplyPacket =
                                    static_cast<CoordinatorReplyPacket*> (recvPacket);
                            if (m_debugCellDiscovery) {
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::findCoordinator() - endpoint=%s:%d\n"),
                                        cellReplyPacket->getCoordinatorEndpoint()->getHost().c_str(),
                                        cellReplyPacket->getCoordinatorEndpoint()->getPort()));
                            }
                            //delete packet;
                            return new Endpoint(cellReplyPacket->getCoordinatorEndpoint());
                        }
                        default:
                        {
                            //delete packet;

                            break;
                        }
                    }
                }
            }
        }
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) CellDiscovery::findCoordinator(): timeout\n")));   
        SleepHelper::sleepRandom(REQUEST_CELL_TIMEOUT_INTERVAL_MS);
    }
    return 0;
}

CellReply* CellDiscovery::requestCell(UUIDPtr& cellID) {
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - before lock\n")));
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, 0);
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - after lock\n")));
    //RequestCellPacket *packet = 0;
    CellReply* reply = 0;
    //m_channel->synchronous();

    int timeoutPerTry = REQUEST_CELL_TIMEOUT_MS; // / REQUEST_CELL_MAX_TRIES;
    ACE_Time_Value overallTimeout = ACE_OS::gettimeofday();
    ACE_Time_Value overallTimeoutDelta;
    overallTimeoutDelta.msec(MAX_WAIT_TIME/*REQUEST_CELL_TIMEOUT_MS*/);
    overallTimeout += overallTimeoutDelta;
    //flag if we catch a coordinator peer    
    RequestCellPacket* cellRequestPacket = 0;
    while (ACE_OS::gettimeofday() < overallTimeout) {
        //if (m_debugCellDiscovery) {
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - Inside loop\n")));
        //}

        ACE_Time_Value* sendTimeout = 0;

        RequestCellPacket* packet = new RequestCellPacket(m_uuid,
                cellID, UUID::INVALID_ID,
                //CellID::ROOT_CELL_ID_UUIDPTR,
                CellID::INVALID_CELL_ID_UUIDPTR,
                0);

        int ret = m_channel->send(packet, sendTimeout);
        if (DOUBLE_SEND_KERNEL_2_6_39_BUG) {
            /*if (ret == -1) {
                delete packet;
                return 0;
            }*/
            ret = m_channel->send(packet, sendTimeout);
        }
        delete packet;
        /*if (ret == -1) {
            return 0;
        }*/
        //SthenoPacket* recvPacket = 0;

        ACE_Time_Value timeout = ACE_OS::gettimeofday();
        ACE_Time_Value delta;
        delta.msec(timeoutPerTry);
        timeout += delta;

        while (ACE_OS::gettimeofday() < timeout) {
            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)recv\n");
            //ACE_Time_Value receiveTimeout(0, 1000);
            ACE_Time_Value receiveTimeout = timeout - ACE_OS::gettimeofday();
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)now=%lu timeout=%lu\n"), receiveTimeout.msec(), timeout.msec()));
            SthenoPacket* recvPacket = 0;
            m_channel->receivePacket(recvPacket, &receiveTimeout);
            //if (m_debugCellDiscovery) {
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - after recv\n")));
            //}
            if (recvPacket != 0) {
                if (recvPacket->getPacketHeader()->getSrcPID()->compare(*(m_uuid.get())) == 0) {
                    //if (m_debugCellDiscovery) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - own packet :%d\n"), recvPacket->getPacketHeader()->getType()));
                    //}
                    delete recvPacket;
                    recvPacket = 0;
                    continue;
                } else {
                    //if (m_debugCellDiscovery) {
                    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - packet type(%d)\n"),
                            //recvPacket->getPacketHeader()->getType()));
                    //}
                    switch (recvPacket->getPacketHeader()->getType()) {
                        case REQUEST_CELL_REPLY_PACKET:
                        {
                            RequestCellReplyPacket* cellReplyPacket =
                                    static_cast<RequestCellReplyPacket*> (recvPacket);
                            //if (m_debugCellDiscovery) {
                            /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - sender(%s) (%s,%@,%@,%@,%@,%@)\n"),
                                    cellReplyPacket->getPacketHeader()->getSrcPID()->toString().c_str(),
                                    cellReplyPacket->getCell()->getCellID()->toString().c_str(),
                                    cellReplyPacket->getCoordinatorUUID().get(),
                                    cellReplyPacket->getCoordinatorEndpoint().get(),
                                    cellReplyPacket->getParentCell().get(),
                                    cellReplyPacket->getParentUUID().get(),
                                    cellReplyPacket->getParentCoordinatorEndpoint().get()));*/
                            //}
                            reply = new CellReply(cellReplyPacket->getCell(),
                                    cellReplyPacket->getCoordinatorUUID(),
                                    cellReplyPacket->getCoordinatorEndpoint(),
                                    cellReplyPacket->getParentCell(),
                                    cellReplyPacket->getParentUUID(),
                                    cellReplyPacket->getParentCoordinatorEndpoint(),
                                    cellReplyPacket->getDiscoverySAPInfo(),
                                    cellReplyPacket->getMeshSAPInfo(),
                                    cellReplyPacket->getFTSAPInfo()
                                    );
                            /*if (packet != 0) {
                                delete packet;
                                packet = 0;
                            }*/
                            if (cellRequestPacket != 0) {
                                delete cellRequestPacket;
                                cellRequestPacket = 0;
                            }
                            return reply;
                        }
                        case REQUEST_CELL_PACKET:
                        {
                            RequestCellPacket* cellRequestPacketTemp =
                                    static_cast<RequestCellPacket*> (recvPacket);
                            //other peer is binding
                            //SleepHelper::sleepRandom(REQUEST_CELL_TIMEOUT_INTERVAL_MS);
                            //if (cellRequestPacket != 0) {
                            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)******************************************\n");
                            //  if (cellRequestPacketTemp->getPacketHeader()->getSrcPID()->
                            //          compare(*(cellRequestPacket->getPacketHeader()->getSrcPID().get())) == 0) {
                            //the same peer, packet retransmission
                            //      delete cellRequestPacketTemp;
                            //      cellRequestPacketTemp = 0;
                            //break;
                            //      continue;
                            //  }
                            //} else {
                            //cellRequestPacket = cellRequestPacketTemp;
                            //}
                            //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)****************************************** -------------------\n");
                            int compareUUID = cellRequestPacketTemp->getPacketHeader()->getSrcPID()->compare(*(m_uuid.get()));
                            //if (m_debugCellDiscovery) {
                            /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - detected other peer: %s %s compare=%d\n"),
                                    cellRequestPacketTemp->getPacketHeader()->getSrcPID()->toString().c_str(),
                                    m_uuid->toString().c_str(), compareUUID));*/
                            //}
                            if (compareUUID < 0) {
                                //if (m_debugCellDiscovery) {
                                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() - sleeping for %d msec\n"), MAX_WAIT_TIME));
                                //}
                                SleepHelper::sleep(MAX_WAIT_TIME);
                                //SleepHelper::sleepRandom(REQUEST_CELL_TIMEOUT_INTERVAL_MS);
                                //must repeat process to detect the other peer
                                overallTimeout = ACE_OS::gettimeofday();
                                overallTimeoutDelta.msec(MAX_WAIT_TIME); //REQUEST_CELL_TIMEOUT_MS);
                                overallTimeout += overallTimeoutDelta;
                                //delete packet;
                            }

                            break;

                        }
                        case NOTIFY_PACKET:
                        {
                            CellNotifyPacket* notifyPacket =
                                    static_cast<CellNotifyPacket*> (recvPacket);
                            delete recvPacket;
                            break;
                        }
                        case REQUEST_COORDINATOR_PACKET:
                        {
                            RequestCoordinatorPacket* requestCoordPacket =
                                    static_cast<RequestCoordinatorPacket*> (recvPacket);
                            delete recvPacket;
                            break;
                        }

                        default:
                        {
                            ACE_DEBUG((LM_DEBUG,
                                    ACE_TEXT("(%t|%T)ERROR: CellDiscovery unknown packet type=%d\n"),
                                    recvPacket->getPacketHeader()->getType()));
                            delete recvPacket;
                            break;
                        }
                    }
                }
            } else {//no reply
                if (cellRequestPacket != 0) {
                    delete cellRequestPacket;
                    cellRequestPacket = 0;
                }
                return 0;
            }
        }
        /*if (packet != 0) {
            delete packet;
            packet = 0;
        }*/
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T) CellDiscovery::requestCell(): timeout\n")));   
        SleepHelper::sleepRandom(REQUEST_CELL_TIMEOUT_INTERVAL_MS);
    }
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::requestCell() outside loop %@\n"), reply));
    if (cellRequestPacket != 0) {

        delete cellRequestPacket;
    }
    return reply;
}

int CellDiscovery::asynchronous() {
    ACE_GUARD_RETURN(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock, -1);
    int ret = m_channel->asynchronous(true, true);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::asynchronous() ret=%d end\n"), ret));
    return ret;
}

void CellDiscovery::open(CellAddrPtr& cellAddr,
        Cell* cell) throw (CellDiscoveryException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::open(CellAddrPtr& cellAddr,Cell* cell\n")));
    //m_cellID = cellAddr->getCellID();
    //m_channel = new CellDiscoveryChannel(this, true, true, false);
    m_channel = new CellDiscoveryChannel(this, false, false, false);
    //EM setup
    m_tpc = new ThreadPerConnection();
    //open the EM
    //m_tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    m_tpc->bind(m_channel);
    m_tpc->open(reserve, cpuQoS);
    //end of EM setup
    //binds the channel to the ec, sets the reactor
    //m_tpc->bind(m_channel);
    //opens the channel with the proper settings
    //"lo"
    ACE_INET_Addr addr(cellAddr->getPort(), cellAddr->getIP().c_str());
    m_channel->open(addr, cellAddr->getITF().c_str(), 1);
    if (cell != 0) {
        this->attach(cell);
    } else {
        printf("Cell=============================0\n");
    }
}

void CellDiscovery::open(Cell* cell) throw (CellDiscoveryException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::open(CEll* cell)\n")));
    // m_cell = cell;
    //m_cellID = CellID::ROOT_CELL_ID;
    //we first create the channel, later we bind it to the desired
    //mcast addr
    //m_channel = new CellDiscoveryChannel(this, true, true, false);
    m_channel = new CellDiscoveryChannel(this, false, false, false);
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
    m_tpc = new ThreadPerConnection();
    //open the EM
    //m_tpc->open();
    CPUQoS* cpuQoS = new CPUPriorityQoS(CPUQoS::SCHEDULE_RT_DEFAULT, CPUQoS::MAX_RT_PRIO);
    CPUReservation* reserve = 0;
    if (getQoSManager() != 0) {
        reserve = getQoSManager()->createCPUReservation("HRT", cpuQoS);
    }
    m_tpc->bind(m_channel);
    m_tpc->open(reserve, cpuQoS);

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::open(CEll* cell) after open tpc\n")));
    //end of EM setup

    //binds the channel to the ec, sets the reactor
    //m_tpc->bind(m_channel);
    //opens the channel with the proper settings
    //"lo"
    ACE_INET_Addr addr(port, ip.c_str());
    int ret = m_channel->open(addr, itf.c_str(), 1);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::open(CEll* cell) after channel open=%d\n"), ret));
    if (cell != 0) {

        this->attach(cell);
    }
}

void CellDiscovery::handleDiscoveryPacket(SthenoPacket* packet) {
    if (m_cell != 0) {
        m_cell->handleDiscoveryPacket(this, packet);
    } else {

        delete packet;
    }
}

void CellDiscovery::attach(Cell* cell) throw (CellDiscoveryException&) {
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::attach()\n")));
    if (m_cell != 0) {

        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: CellDiscovery::attach() m_cell!=0!\n")));
        throw CellDiscoveryException();
    }
    m_cell = cell;
    cell->getCellID(m_cellID);
    int ret = m_channel->asynchronous(true, true);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: CellDiscovery::attach() ret=%d end\n"), ret));
}

void CellDiscovery::deattach() {

    m_channel->synchronous();
    m_cell = 0;
}

Cell* CellDiscovery::getCell() {

    return m_cell;
}
//Keep Alive

void CellDiscovery::sendGroupKeepAlive() {
    UInt meshSize = 1;
    CellNotifyPacket *packet = 0;
    //TODO: fix this
    /*if(!m_cellID.null()){
        UUIDPtr cellID(new CellID(m_cellID.get()));
    }*/
    /*ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)CellDiscovery:sendGroupKeepAlive() SRC(%s,%s) DST(%s,%s)", m_cell->getPID().get()->toString().c_str(), CellID::INVALID_CELL_ID->toString().c_str(),
            UUID::INVALID_ID->toString().c_str(), m_cell->getCellID()->toString().c_str());
    packet = new CellNotifyPacket(m_cell->getPID(),
            cellID, UUID::INVALID_ID, cellID, meshSize, m_keepAliveCounter++);
    m_channel->send(packet, (ACE_Time_Value*) 0);*/
}
