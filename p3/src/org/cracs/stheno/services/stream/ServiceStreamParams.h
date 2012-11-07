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
 * File:   ServiceStreamParams.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 12:13 PM
 */

#ifndef SERVICESTREAMPARAMS_H
#define	SERVICESTREAMPARAMS_H

#include <stheno/service/ServiceParams.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/services/stream/StreamQoS.h>

class ServiceStreamParams : public ServiceParams {
public:
    ServiceStreamParams(
            const UUIDPtr& sid,
            const UUIDPtr& sensorUUID,
            EndpointPtr& endpoint,
            StreamQoS* qos);
    ServiceStreamParams(
            const UUIDPtr& sid,
            const UUIDPtr& sensorUUID,
            EndpointPtr& endpoint,
            StreamQoS* qos,
            FTServiceParamsPtr& ftParams);
    
    ServiceStreamParams(ServiceParams* params);
    
    virtual ~ServiceStreamParams();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        deserializeMetadata(inputStream);
        UInt size = 0;
        inputStream.read_ulong(size);
        deserializeQuerySpecialization(inputStream);
    }

    StreamQoS* getQoS();
    UUIDPtr& getSensorUUID();
    EndpointPtr& getEndpoint();
    
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
        //ServiceParams::serialize(outputStream);
        m_sensorUUID->serialize(outputStream);
        m_endpoint->serialize(outputStream);
        m_qos->serialize(outputStream);
    }

protected:
    StreamQoS* m_qos;
    UUIDPtr m_sensorUUID;
    EndpointPtr m_endpoint;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
        ByteInputStream inputStream(mb);
        deserializeQuerySpecialization(inputStream);
    }

    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
        //ServiceParams::deserialize(inputStream);
        UUID* sensorUUID = new UUID(inputStream);
        m_sensorUUID.reset(sensorUUID);
        Endpoint* endpoint = new Endpoint(inputStream);
        printf("Endpoint=%s\n", endpoint->toString().c_str());
        m_endpoint.reset(endpoint);
        if (m_qos != 0) {
            delete m_qos;
        }
        m_qos = new StreamQoS(inputStream);

    }

};

#endif	/* SERVICESTREAMPARAMS_H */

