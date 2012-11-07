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
#include "Endpoint.h"
#if !defined (__ACE_INLINE__)
#include <euryale/common/Endpoint.inl>
#endif /* __ACE_INLINE__ */

/*namespace pt {

namespace efacec {

namespace rto {

namespace runtime {*/
Endpoint::Endpoint(const EndpointPtr& endpointPtr) throw(EndpointException&):
m_stringRep(0),m_qos(0),m_type(0),m_port(0)
{
    if(endpointPtr.null()){
        printf("KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKkk\n");
        throw EndpointException(EndpointException::NULL_ENDPOINTPTR);
    }
    if((endpointPtr->m_qos)!=0){
        m_qos = new QoSEndpoint(*(endpointPtr->m_qos));
    }
    m_host = endpointPtr->m_host;
    m_port = endpointPtr->m_port;
    m_addr = endpointPtr->m_addr;
    m_type = endpointPtr->m_type;
}

Endpoint::Endpoint(EndpointType type, String& host, UShort port, QoSEndpoint* qos) :
m_type(type), m_host(host), m_port(port), m_qos(qos), m_stringRep(0), m_addr(port, host.c_str()) {
    if(qos==0){
        m_qos = new QoSEndpoint();
    }
}

Endpoint::~Endpoint() {
    if (m_stringRep != 0){
        delete m_stringRep;
    }
    if(m_qos != 0) {
        delete m_qos;
    }
}

/*}

}

}

}*/

