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
 * File:   SAP.h
 * Author: rmartins
 *
 * Created on September 17, 2010, 2:30 PM
 */

#ifndef SAP_H
#define	SAP_H
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/net/endpoint/SAPInfo.h>
#include <euryale/Types.h>
#include <euryale/net/EventHandler.h>
#include <euryale/net/AbstractStreamChannel.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <euryale/net/StreamChannelGroup.h>
#include <euryale/threading/tpc/ThreadPerConnection.h>
#include <euryale/net/NetworkUtils.h>

template <class ACCEPTOR>
class SAP {
public:
    static const int MAX_PORT_BIND_ATTEMPTS = 500000;

    SAP() {}

    virtual void open(String& itf, list<QoSEndpoint>& qosList) throw (EndpointException&) {
        ACE_INET_Addr* addr = NetworkUtils::get_ip_interface(itf.c_str());
        if (addr == 0) {
            throw EndpointException("Invalid interface");
        }
        //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - interface: %s IP=%s\n"), itf.c_str(), addr->get_host_addr()));
        String ip = addr->get_host_addr();
        delete addr;

        list<QoSEndpoint>::iterator iter = qosList.begin();
        SAPInfo* sapInfo = new SAPInfo();
        while (iter != qosList.end()) {
            ACCEPTOR* acceptor = createAcceptor();
            try {
                initAcceptor(acceptor, ip, *iter);
                m_acceptorList.push_back(acceptor);                
                sapInfo->addEndpoint(*(acceptor->getEndpoint().get()));
            } catch (EndpointException& ex) {
                delete sapInfo;
                close();
                throw ex;
            }
            iter++;
        }
        m_sapInfo.reset(sapInfo);
        
    }

    virtual ~SAP() {
        close();
    }

    virtual void close() {
        typename list<ACCEPTOR*>::iterator iter = m_acceptorList.begin();
        while (iter != m_acceptorList.end()) {
            delete *iter;
            iter++;
        }
        m_acceptorList.clear();
    }
    
    SAPInfoPtr& getSAPInfo(){
        return m_sapInfo;
    }

    list<ACCEPTOR*>& getAcceptorList(){
        return m_acceptorList;
    }
    
protected:
    list<ACCEPTOR*> m_acceptorList;
    SAPInfoPtr m_sapInfo;

    virtual ACCEPTOR* createAcceptor() = 0;

    void initAcceptor(ACCEPTOR* acceptor, String& ip, QoSEndpoint& qos) throw (EndpointException&) {
        int maxBinds = MAX_PORT_BIND_ATTEMPTS;
        bool bindOk = false;
        for (int i = 0; i < maxBinds; i++) {
            UShort port = NetworkUtils::randomPort();
            //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SAP<>::initAcceptor() try No %d\n"),port));
            EndpointPtr endpoint(
                    //new Endpoint(Endpoint::TCP, new String("127.0.0.1"),
                    new Endpoint(Endpoint::TCP, ip,
                    port, new QoSEndpoint(qos)));
            /*ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SAP<>::initAcceptor() - %@ ********Endpoint(%s)******\n"),
                        acceptor,endpoint->toString().c_str()));*/
            if (acceptor->open(endpoint) == 0) {
                //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: Cell::createSAP() - acceptor ok\n")));
                bindOk = true;
                
                break;
            }
        }
        if (!bindOk) {
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t|%T)INFO: SAP<>::initAcceptor() - bind nok\n")));
            throw EndpointException("Unable to bind");
        }
    }

};



#endif	/* SAP_H */

