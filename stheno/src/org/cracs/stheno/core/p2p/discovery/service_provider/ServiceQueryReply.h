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
 * File:   PeerQueryReply.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 13, 2010, 2:40 PM
 */

#ifndef SERVICEQUERYREPLY_H
#define	SERVICEQUERYREPLY_H

#include <list>
using std::list;

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

class ServiceQueryReply : public DiscoveryQueryReply {
public:
    static const UInt SERVICE_QUERY_REPLY = 0xDA908912;
    
    ServiceQueryReply();
    ServiceQueryReply(list<ServiceInstanceInfoPtr>& instanceList);
    ServiceQueryReply(DiscoveryException* ex);
    ServiceQueryReply(DiscoveryQueryReply* query);
    virtual ~ServiceQueryReply();

    list<ServiceInstanceInfoPtr>& getServiceInfo() {
        return m_list;
    }
    //[(iid,[endpoint ...]) ...]

    void add(list<ServiceInstanceInfoPtr>& instanceList) {
        list<ServiceInstanceInfoPtr>::iterator iter = instanceList.begin();
        while (iter != instanceList.end()) {
            if (!isInstanceInList(*iter)) {
                m_list.push_back(*iter);
            }
            iter++;
        }
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        deserializeMetadata(inputStream);
        //delete m_ex if present
        if (m_ex != 0) {
            delete m_ex;
            m_ex = 0;
        }
        //end clear
        Boolean flag = false;
        inputStream.read_boolean(flag);
        if (flag) {
            m_ex = new DiscoveryException(inputStream);
        } else {
            UInt size = 0;
            inputStream.read_ulong(size);
            deserializeQuerySpecialization(inputStream);
        }
    }

    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
        printf("HHHHHHHHHHHHHHHHHHHHHHHH\n");
        if (!isException()) {
            printf("HHHHHHHHHHHHHHHHHHHHHHHH1111111111111111111\n");
            UInt count = m_list.size();
            printf("HHHHHHHHHHHHHHHHHHHHHHHH1111111111111111111    %d\n",count);
            outputStream.write_ulong(count);
            list<ServiceInstanceInfoPtr>::iterator iter = m_list.begin();
            while (iter != m_list.end()) {
                (*iter)->serialize(outputStream);
                iter++;
            }
        }
    }

protected:
    list<ServiceInstanceInfoPtr> m_list;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
        ByteInputStream inputStream(mb);
        deserializeQuerySpecialization(inputStream);
    }

    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
        if (!isException()) {
            UInt count = 0;
            m_list.clear();
            inputStream.read_ulong(count);
            for (int i = 0; i < count; i++) {
                ServiceInstanceInfo* pi = new ServiceInstanceInfo(inputStream);
                ServiceInstanceInfoPtr iPtr(pi);
                m_list.push_back(iPtr);
            }
        }
    }

    bool isInstanceInList(ServiceInstanceInfoPtr& info) {
        list<ServiceInstanceInfoPtr>::iterator iter = m_list.begin();
        while (iter != m_list.end()) {
            if ((*iter)->getIID() == info->getIID()) {
                return true;
            }
            iter++;
        }
        return false;
    }

};

#endif	/* SERVICEQUERYREPLY_H */

/*virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        DiscoveryQueryReply::serialize(outputStream);
        if (!isException()) {
            UInt count = m_list.size();
            outputStream.write_ulong(count);
            list<ServiceInstanceInfoPtr>::iterator iter = m_list.begin();
            while (iter != m_list.end()) {
                (*iter)->serialize(outputStream);
                iter++;
            }
        }
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        DiscoveryQueryReply::deserialize(inputStream);
        if (!isException()) {
            UInt count = 0;
            m_list.clear();
            inputStream.read_ulong(count);
            for (int i = 0; i < count; i++) {
                ServiceInstanceInfo* i = new ServiceInstanceInfo(inputStream);
                ServiceInstanceInfoPtr iPtr(i);
                m_list.push_back(iPtr);
            }
        }

    }
 */