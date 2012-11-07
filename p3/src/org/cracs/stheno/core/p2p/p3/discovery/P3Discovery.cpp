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
 * File:   P3Discovery.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 6, 2010, 11:14 AM
 */

#include "P3Discovery.h"
#include <stheno/core/p2p/p3/mesh/P3Mesh.h>
//#include "SAP.h"
#include <euryale/net/endpoint/SAP.h>

#include <stheno/core/p2p/discovery/DiscoveryQuery.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>

#include <stheno/core/p2p/discovery/DiscoveryProvider.h>
#include <euryale/common/ListHelper.h>
#include <stheno/core/p2p/p3/P3.h>
#include <euryale/common/uuid/PeerLocation.h>
#include <euryale/net/RequestGroup.h>
#include <stheno/core/p2p/p3/discovery/net/DiscoveryRequest.h>

#include <stheno/core/p2p/network/net/stream/StreamChannelSvcHandler.h>
#include <stheno/core/p2p/p3/discovery/general_provider/P3GeneralProvider.h>

P3Discovery::P3Discovery(P3* overlay) :
//ServiceAbstract(id(), ServiceAbstract::P2P_TYPE), m_overlay(overlay), m_sap(0) {
Discovery(overlay), m_sap(0) {
}

P3Discovery::~P3Discovery() {
    close();
}

void P3Discovery::close() {
    printf("P3Discovery::close - 1\n");
    if (m_sap != 0) {
        m_sap->close();
        delete m_sap;
    }
    printf("P3Discovery::close - 2\n");
    Discovery::close();
    printf("P3Discovery::close - 3\n");
    //ListHelper<DiscoveryProviderInfo*>::clearList(m_eventProvider);
    /*ACE_Hash_Map_Manager<int, DiscoveryProviderInfo*,
            ACE_SYNCH_RW_MUTEX>::ITERATOR iter = m_eventProvider.begin();
    while (iter != m_eventProvider.end()) {
        //delete iter->item();
        iter++;
    }
    m_eventProvider.unbind_all();*/
}

/*bool P3Discovery::addDiscoveryProvider(DiscoveryProvider* provider,
        list<UInt>& eventIDList)
throw (ServiceException&) {
    list<UInt>::iterator iter = eventIDList.begin();
    while (iter != eventIDList.end()) {
        DiscoveryProviderInfo* oldValue = findProvider(*iter);
        if (oldValue == 0) {
            DiscoveryProviderInfo* info =
                    new DiscoveryProviderInfo(provider, *iter);
            m_eventProvider.bind(*iter, info);
            iter++;
        } else {
            removeDiscoveryProvider(provider, eventIDList);
            return false;
        }
    }
    return true;
}

bool P3Discovery::removeDiscoveryProvider(DiscoveryProvider* provider,
        list<UInt>& eventIDList) throw (ServiceException&) {
    list<UInt>::iterator iter = eventIDList.begin();
    while (iter != eventIDList.end()) {
        m_eventProvider.unbind(*iter);
        iter++;
    }
    return true;
}*/

/*DiscoveryProviderInfo* P3Discovery::findProvider(UInt providerID) {
    DiscoveryProviderInfo* info;
    if (m_eventProvider.find(providerID, info) == 0) {
        return info;
    }
    return 0;
}*/

void P3Discovery::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {

    //void P3Discovery::open(ServiceParamsPtr& params, int fttype) throw (ServiceException&) {

    ACE_GUARD(ACE_SYNCH_RECURSIVE_MUTEX, ace_mon, m_lock);
    if (isStarting() /*|| isResuming()*/) {
        if (m_sap == 0) {
            list<QoSEndpoint> qosList;
            //QoSEndpoint hrt(QoS::HRT, 50);
            //QoSEndpoint srt(QoS::SRT, 25);
            QoSEndpoint hrt(QoS::HRT, CPUQoS::MAX_RT_PRIO);
            QoSEndpoint srt(QoS::SRT, CPUQoS::MED_RT_PRIO);
            QoSEndpoint be(QoS::BE, CPUQoS::BE_PRIO);
            qosList.push_back(hrt);
            qosList.push_back(srt);
            qosList.push_back(be);
            String itf;
            if (!P3Configuration::instance()->lookupValue("DEFAULT_INTERFACE", itf)) {
                throw ServiceException(ServiceException::INVALID_ARGUMENT);
            }
            m_sap = new P3DiscoverySAP(this);
            m_sap->open(itf, qosList);
        }
    } else {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SuperPeerDiscovery::open(): error\n")));
        throw ServiceException(ServiceException::REGISTRATION_ERROR);
    }
}

/*P3* P3Discovery::getOverlay_i() {
    return static_cast<P3*> (m_overlay);
}

void P3Discovery::handleQueryPacket(P3DiscoverySvcHandler* handler, P3DiscoveryQueryPacket* packet) {
    list<PeerLocation> l;
    //m_meshService->getNeighbors(l);
    getOverlay_i()->getP3MeshService()->getNeighbors(l);
    list<PeerLocation>::iterator iter = l.begin();
    UUIDPtr fid(new UUID(*(handler->getCellID().get())));
    PeerLocation loc(handler->getPID(), fid);
    while (iter != l.end()) {
        if ((*iter).isEqual(loc)) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: P3Discovery::handleQueryPacket - remove the requesting handler uuid\n")));
            l.remove(loc);
            break;
        }
        iter++;
    }
    if (l.size() == 0) {
        //reply
        DiscoveryQueryReply* queryReply = executeLocalQuery(packet->getQuery().get());
        //P3DiscoveryQueryReplyPacket(ULong packetID, ULong replyPacketID,UUIDPtr& uuid, UUIDPtr& fid, DiscoveryQueryReply* requeyReply);
        P3DiscoveryQueryReplyPacket* replyPacket = new
                P3DiscoveryQueryReplyPacket(0, packet->getPacketHeader()->getPacketID(),
                m_overlay->getP3MeshService()->getUUID(), m_overlay->getP3MeshService()->getFID(), queryReply);
        handler->send((SthenoStreamPacket*&) replyPacket);
        delete packet;
        return;
    }

    //reset the iterator    
    iter = l.begin();
    //Request set, that in turn is also a Requuest
    ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>::ChannelTypePtr handlerPtr;
    //P3DiscoverySvcHandler::channel_ptr handlerPtr;
    m_svcChannels.getChannel(handler->getPID(), handlerPtr);
    DiscoveryRequest* discoveryRequest = new DiscoveryRequest(handlerPtr);

    RequestGroup<P3DiscoveryClientHandler,
            ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr*> requestGroup(0);
    while (iter != l.end()) {
        ChannelManager<ACE_SOCK_Stream, ACE_MT_SYNCH>::ChannelTypePtr channel;
        m_clients.getChannel(iter->getPID(), channel);
        P3DiscoveryClientHandler* clientHandler = static_cast<P3DiscoveryClientHandler*> (channel.get());
        if (clientHandler == 0) {
            ACE_Connector<P3DiscoveryClientHandler, ACE_SOCK_Connector> connector;
            try {
                SAPInfoPtr sapInfo = m_overlay->getP3MeshService()->getDiscoverySAPInfo(iter->getPID());
                QoSEndpoint qosE = handler->getQoSEndpoint();
                Endpoint endpoint;
                sapInfo->getEndpoint(qosE, endpoint);
                clientHandler = new P3DiscoveryClientHandler(iter->getPID(), iter->getFID(), qosE, true, true, false, 0, 0, 0, 0);
                ThreadPerConnection *tpc = new ThreadPerConnection();
                ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
                        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);
                tpc->open();
                tpc->bind(clientHandler);
                connector.reactor(tpc->getResources()->getReactor());


                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connecting...(%s)!\n"), endpoint.getAddr().toString().c_str()));
                if (connector.connect(clientHandler, endpoint.getAddr()) == -1) {
                    ACE_ERROR((LM_ERROR, ACE_TEXT("(%T)%@\n"),
                            ACE_TEXT("(%T)connect failed:")));
                    clientHandler->close();
                    clientHandler = 0;
                    delete clientHandler;
                    //throw ServiceException("Error connecting to coordinator!");
                } else {
                    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell:joinNonCoordinator() - Connect OK!\n")));

                }
            } catch (ServiceException& ex) {

            }
        }//else we already have an iterator

        if (clientHandler != 0) {
            P3DiscoveryQueryPacket* packet = new P3DiscoveryQueryPacket(
                    0, m_overlay->getUUID(), m_overlay->getP3MeshService()->getFID(),
                    packet->getQoS(), new DiscoveryQuery(*(packet->getQuery().get())));
            ChannelRequestEngine<P3DiscoveryClientHandler, SthenoStreamPacket*>::RequestPtr* request =
                    clientHandler->sendRequest(packet, 0);

            requestGroup.setFuture(request);

        } else {
            //we failed to get a channel    
        }

        iter++;
    }





}
 */

DiscoveryQueryReply* P3Discovery::executeLocalQuery(DiscoveryQuery* query,
        DiscoveryQoS* qos) {
    DiscoveryProviderInfo* provider = this->findProvider(query->getType());
    if (provider == 0) {
        String msg("No provider");
        DiscoveryException* ex = new DiscoveryException(msg);
        DiscoveryQueryReply* reply = new DiscoveryQueryReply(query->getProvider(), query->getType(), ex);
        return reply;
    } else {
        return provider->getProvider()->executeQuery(query, qos);
    }
    return 0;
}

void P3Discovery::getSAP(SAPInfoPtr& sapInfo) {
    sapInfo = m_sap->getSAPInfo();
}

/*int P3Discovery::handleNewClient(StreamChannelAcceptor* acceptor){
    StreamChannelSvcHandler* client;
    ThreadPerConnection *tpc = new ThreadPerConnection();
    ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex>* tpcPrt = new
        ACE_Strong_Bound_Ptr<ThreadPerConnection, ACE_Recursive_Thread_Mutex > (tpc);

    ACE_NEW_RETURN(client,
                   StreamChannelSvcHandler(new P3DiscoveryPacketFactory(),
            0, 
            false,
            (ExecutionModelPtr*) tpcPrt, 0, 0, 0), -1);
    
    auto_ptr<StreamChannelSvcHandler> p(client);

    if (acceptor->getAcceptorSock().accept(client->peer()) == -1)
        ACE_ERROR_RETURN((LM_ERROR,
                         ACE_TEXT("(%T) failed to accept\n")),
                         -1);
    p.release();
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO P3Discovery::handleNewClient() - new connection %d!\n"), client->get_handle()));    
    //m_svcChannels.add(client);    
    tpc->open();
    tpc->bind(client);
    client->open();
    acceptor->getChannelManager().add(client);
    //client->asynchronous(true,true);
    client->asynchronous(this,true);
    return 0;    
}

int P3Discovery::handleCloseClient(StreamChannelAcceptor* acceptor, StreamChannel* channel){
    //m_scvHandlers.remove(channel);
}
 */

/*
P3* P3Discovery::getOverlay_i() {
    return static_cast<P3*> (m_overlay);
}*/


const char* P3Discovery::id() {
    return "AFC8425E5454D05E991866840F8A65CD";
}

Overlay* P3Discovery::getOverlay() {
    return (Overlay*) m_overlay;
}

ServiceAbstract* P3Discovery::duplicate() throw (ServiceException&) {
    throw ServiceException(ServiceException::SERVICE_WITHOUT_IMPL);
}

//SAPInfoPtr& P3Discovery::getSAP();

QoSResources* P3Discovery::calculateQoSResources(ServiceParamsPtr& params) {
    return 0;
}

const char* P3Discovery::getName() {
    return "P3Discovery";
}