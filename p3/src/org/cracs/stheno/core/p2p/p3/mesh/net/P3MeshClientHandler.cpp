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
 * File:   P3MeshClientHandler.cpp
 * Author: rmartins
 * 
 * Created on September 30, 2010, 6:25 PM
 */

#include "P3MeshClientHandler.h"
#include "packet/P3MeshCreateServiceReplyPacket.h"
#include "packet/P3MeshRemoveServiceReplyPacket.h"

#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshCreateServicePacket.h>
#include <stheno/core/p2p/p3/mesh/net/packet/P3MeshRemoveServicePacket.h>

P3MeshClientHandler::~P3MeshClientHandler() {
    if (m_debugP3MeshClientHandler) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO P3MeshClientHandler::~P3MeshClientHandler() %@\n"), this));
    }
    close();
}

P3MeshClientHandler::P3MeshClientHandler() {
    m_debugP3MeshClientHandler = RuntimeConfiguration::isClassDebugged("P3MeshClientHandler");
    throw -1;
}

P3MeshClientHandler::P3MeshClientHandler(UUIDPtr& uuid, UUIDPtr& fid,
        QoSEndpoint& qosE, bool asyncRead, bool asyncWrite,
        ExecutionModelPtr* ec,
        NetReservation* reserve,
        NetQoS* qos,
        CloseListener<ACE_SOCK_Stream, ACE_MT_SYNCH>* closeListener) :
PacketChannel<SthenoStreamPacket, SthenoStreamHeader, //P3MeshPacketFactory,
ACE_SOCK_Stream, ACE_MT_SYNCH>(new P3MeshPacketFactory(), asyncRead, asyncWrite,
ec, reserve, qos, closeListener), m_uuid(uuid), m_fid(fid), m_qos(qosE) {
    m_debugP3MeshClientHandler = RuntimeConfiguration::isClassDebugged("P3MeshClientHandler");
    //m_requests = new ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>();
}

ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr* P3MeshClientHandler::sendRequest(SthenoStreamPacket* packet,
        ACE_Time_Value* timeout) {
    ACE_Message_Block* mb = serializePacket(packet);
    UInt id = ((SthenoStreamHeader*) (packet->getPacketHeader()))->getPacketID();
    ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr* request =
            new ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr(
            new ChannelRequest<P3MeshClientHandler, SthenoStreamPacket*>(this, &m_requests, id, timeout));
    m_requests.bind(request);
    int ret = AbstractStreamChannel<ACE_SOCK_Stream, ACE_MT_SYNCH>::send(mb, timeout);
    ACE_Message_Block::release(mb);
    //ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: CellClientHandler:sendRequest() - register ID(%d).\n"), id));
    if (ret == -1) {
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3MeshClientHandler:sendRequest() - ID(%d)\n"), id));
        delete request;
        return 0;
    }
    return request;
}

int P3MeshClientHandler::handlePacket(SthenoStreamPacket* packet) {
    switch (packet->getPacketHeader()->getType()) {
        case P3MeshCreateServiceReplyPacket::P3MESH_CREATESERVICERPELY_PACKET:
        {
            P3MeshCreateServiceReplyPacket* createServicePacket = static_cast<P3MeshCreateServiceReplyPacket*> (packet);
            UInt id = createServicePacket->getReplyPacketID();
            if (m_requests.setReply(id, packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: P3MeshClientHandler::handlePacket() - failed to set future ID(%d) P3MeshCreateServiceReplyPacket\n"), id));
                delete packet;
                return -1;
            }
            return 0;
        }
        case P3MeshRemoveServiceReplyPacket::P3MESH_REMOVESERVICERPELY_PACKET:
        {
            P3MeshRemoveServiceReplyPacket* removeServicePacket = static_cast<P3MeshRemoveServiceReplyPacket*> (packet);
            UInt id = removeServicePacket->getReplyPacketID();
            if (m_requests.setReply(id, packet) == -1) {
                ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)ERROR: P3MeshClientHandler::handlePacket() - failed to set future ID(%d) P3MeshRemoveServiceReplyPacket\n"), id));
                delete packet;
                return -1;
            }
            return 0;
        }
        default:
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)ERROR: P3MeshClientHandler::handlePacket() - packet not known(%d)\n"), packet->getPacketHeader()->getType()));
            delete packet;
            return 0;
    }
    //}
    return 0;
}

int P3MeshClientHandler::createService(ServiceParamsPtr& params, UUIDPtr& iid) {
    P3MeshCreateServicePacket* createServicePacket = new P3MeshCreateServicePacket(params);
    this->sendPacket(createServicePacket, 0);
    delete createServicePacket;
    
    SthenoStreamPacket* replyAbs = 0;
    this->receivePacket(replyAbs, 0);    
    P3MeshCreateServiceReplyPacket* reply = static_cast<P3MeshCreateServiceReplyPacket*> (replyAbs);
    if(reply == 0){
        return -1;
    }
    if(reply->isException()){
        delete reply;
        return -1;
    }
    iid = reply->getIID();
    printf("Service created!!! IID=%s\n", reply->getIID()->toString().c_str());
    delete reply;
    //ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr* request = this->sendRequest(createServicePacket, 0);
    //list<SthenoStreamPacket*>* l = (*request)->waitFuture(0);
    return 0;
}

void P3MeshClientHandler::removeService(UUIDPtr& sid, UUIDPtr& iid) {
    P3MeshRemoveServicePacket* removeServicePacket = new P3MeshRemoveServicePacket(sid, iid);
    ChannelRequestEngine<P3MeshClientHandler, SthenoStreamPacket*>::RequestPtr* request = this->sendRequest(removeServicePacket, 0);
    list<SthenoStreamPacket*>* l = (*request)->waitFuture(0);
}