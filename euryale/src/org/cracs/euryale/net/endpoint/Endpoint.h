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
#ifndef ENDPOINT_H_
#define ENDPOINT_H_
#include <ace/Containers_T.h>
#include <ace/Synch_Traits.h>
#include <ace/Bound_Ptr.h>
#include <ace/INET_Addr.h>

#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>
#include <euryale/common/io/OutputStream.h>
#include <euryale/common/io/InputStream.h>

#include <euryale/qos/QoS.h>

#include <list>
using std::list;

#include <iostream>
#include <sstream>
using namespace std;

#include <euryale/net/endpoint/exception/EndpointException.h>
#include <euryale/qos/cpu_qos/CPUQoS.h>

class QoSEndpoint : public Serializable {
public:

    QoSEndpoint() :
    m_rep(0), m_priority(0), m_type(-1) {
    }

    QoSEndpoint(InputStream& inputStream) :
    m_rep(0), m_priority(0), m_type(-1) {
        this->deserialize(inputStream);
    }

    QoSEndpoint(UShort type, UShort priority) :
    m_rep(0), m_priority(priority), m_type(type) {
    }

    QoSEndpoint(const QoSEndpoint& qos) :
    m_rep(0) {
        m_type = qos.m_type;
        m_priority = qos.m_priority;
    }

    virtual ~QoSEndpoint() {
        if (m_rep != 0) {
            delete m_rep;
        }
    }

    int getScheduleType() {
        switch (m_type) {
            case QoS::HRT:
            case QoS::SRT:
                return CPUQoS::SCHEDULE_FIFO;
                //return CPUQoS::SCHEDULE_RR;
            case QoS::BE:
            default:
                return CPUQoS::SCHEDULE_OTHER;
        }
    }

    ACE_INLINE void serialize(OutputStream& outputStream) throw (SerializationException&) {
        outputStream.write_ushort(m_type);
        outputStream.write_ushort(m_priority);
    }

    ACE_INLINE void deserialize(InputStream& inputStream) throw (SerializationException&) {
        inputStream.read_ushort(m_type);
        inputStream.read_ushort(m_priority);
    }

    /*virtual UInt getSerializationSize(Alignment* align = 0) throw (SerializationException&) {
        if (align == 0 || align->getType() == Alignment::ZERO_ALIGNMENT) {
            return sizeof (UShort);
        }
        throw new SerializationException(SerializationException::GENERAL_ERROR);
    }*/

    void dump(void) const {
        //ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)QoSEndpoint(%d)"), m_priority));
        //ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
    }

    bool operator==(const QoSEndpoint& node) {
        return isEqual(node);
    }

    bool isEqual(const QoSEndpoint& node) {
        return (m_type == node.m_type) && (m_priority == node.m_priority);
    }

    String& toString() {
        if (m_rep == 0) {
            stringstream ss;
            ss << "QoSEndpoint(";
            switch (m_type) {
                case QoS::HRT:
                {
                    ss << "HRT,";
                    break;
                }
                case QoS::SRT:
                {
                    ss << "SRT,";
                    break;
                }
                case QoS::BE:
                {
                    ss << "BE,";
                    break;
                }

            }
            ss << m_priority << ")";
            //ss << "QoSEndpoint(" << m_priority << ")";
            m_rep = new String(ss.str().c_str());

        }
        return *m_rep;
    }

    const UShort getType() const {
        return m_type;
    }

    const UShort getPriority() const {
        return m_priority;
    }

protected:
    UShort m_type;
    UShort m_priority;

private:
    String* m_rep;
};

class Endpoint;
typedef ACE_Strong_Bound_Ptr<Endpoint, ACE_SYNCH_MUTEX> EndpointPtr;

class Endpoint : public Serializable {
public:

    enum EndpointType {
        TCP = 0x01,
        UDP = 0x03,
        BCAST = 0x04,
        MCAST = 0x05,
        SSL = 0x02
    };

    Endpoint(const EndpointPtr& endpointPtr) throw (EndpointException&);

    Endpoint(EndpointType type, String& host, UShort port, QoSEndpoint* qos);

    Endpoint() :
    m_stringRep(0),m_qos(0),m_type(0),m_port(0) {
    }

    Endpoint(InputStream& input) :
    m_stringRep(0),m_qos(0),m_type(0),m_port(0) {
        //m_host = new String();
        m_qos = new QoSEndpoint();
        m_port = 0;
        deserialize(input);
        m_addr.set(m_port, m_host.c_str());
    }

    Endpoint(const Endpoint& ep) :
    m_stringRep(0),m_qos(0),m_type(0),m_port(0) {
        if (ep.m_qos!=0) {
            m_qos = new QoSEndpoint(*ep.m_qos);
        } else {
            m_qos = new QoSEndpoint();
        }
        //m_host = new String(*ep.m_host);
        m_host = ep.m_host;
        m_port = ep.m_port;
        m_addr = ep.m_addr;
        m_type = ep.m_type;
    }

    Endpoint* duplicate() {
        Endpoint* endpoint = new Endpoint((EndpointType) m_type, m_host, m_port, new QoSEndpoint(*m_qos));
        return endpoint;
    }

    virtual ~Endpoint();


    ACE_INLINE virtual void serialize(OutputStream& outputStream) THROW(SerializationException&);
    ACE_INLINE virtual void deserialize(InputStream& inputStream) THROW(SerializationException&);
    //ACE_INLINE virtual UInt getSerializationSize(Alignment* align = 0) throw (SerializationException&);

    void dump(void) const {
        //ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T)Endpoint(%d,%s,%d,"), m_type, m_host.c_str(), m_port));
        m_qos->dump();
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%T))")));
        //ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
    }

    bool operator==(const Endpoint& node) {
        return isEqual(node);
    }

    void operator=(const Endpoint& ep) {
        if (m_qos != 0) {
            delete m_qos;
            m_qos = 0;
        }
        if (ep.m_qos != 0) {
            m_qos = new QoSEndpoint(*ep.m_qos);
        }
        m_host = ep.m_host;
        m_port = ep.m_port;
        m_addr = ep.m_addr;
        m_type = ep.m_type;
    }

    bool isEqual(const Endpoint& node) {
        return (m_type == node.m_type && m_host.compare(node.m_host) == 0
                && m_port == node.m_port && *m_qos == *node.m_qos);
    }

    QoSEndpoint* getQoS() {
        return m_qos;
    }

    String& toString() {
        if (m_stringRep == 0) {
            stringstream ss;
            ss << "Endpoint(" << (int) m_type << ":" << m_host << ":" << m_port
                    << ":" << m_qos->toString() << ")";
            m_stringRep = new String(ss.str().c_str());
        }
        return *m_stringRep;
    }

    String& getHost() {
        return m_host;
    }

    UShort getPort() {
        return m_port;
    }

    ACE_INET_Addr& getAddr() {
        return m_addr;
    }

protected:
    Byte m_type;
    String m_host;
    UShort m_port;
    QoSEndpoint* m_qos;
    ACE_INET_Addr m_addr;

private:
    String* m_stringRep;

    /*public:
        Endpoint* next_;
        Endpoint* prev_;
     */
};

//typedef ACE_Double_Linked_List<Endpoint> EndpointList;
typedef ACE_Strong_Bound_Ptr<Endpoint, ACE_SYNCH_MUTEX> EndpointPtr;


#if defined (__ACE_INLINE__)
#include <euryale/net/endpoint/Endpoint.inl>
#endif /* __ACE_INLINE__ */


#endif /*ENDPOINT_H_*/

