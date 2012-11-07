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
 * File:   ServiceActuatorParams.h
 * Author: rmartins
 *
 * Created on October 26, 2010, 12:13 PM
 */

#ifndef SERVICEActuatorPARAMS_H
#define	SERVICEActuatorPARAMS_H

#include <stheno/service/ServiceParams.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/services/actuator/ActuatorQoS.h>
#include <stheno/core/p2p/discovery/general_provider/SensorInfo.h>

/*class SensorData : public Serializable {
public:

    SensorData(UUIDPtr& uuid, EndpointPtr& endpoint) :
    m_uuid(uuid), m_endpoint(endpoint) {
    }

    SensorData(const SensorData& orig) :
    m_uuid(orig.m_uuid), m_endpoint(orig.m_endpoint) {
    }

    SensorData(InputStream& inputStream) {
        deserialize(inputStream);
    }

    UUIDPtr& getUUID() {
        return m_uuid;
    }

    EndpointPtr& getEndpoint() {
        return m_endpoint;
    }

    virtual void serialize(OutputStream& outputStream) throw (SerializationException&) {
        m_uuid->serialize(outputStream);
        m_endpoint->serialize(outputStream);
    }

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        UUID* uuid = new UUID(inputStream);
        m_uuid.reset(uuid);
        Endpoint* endpoint = new Endpoint(inputStream);
        m_endpoint.reset(endpoint);
    }

protected:
    UUIDPtr m_uuid;
    EndpointPtr m_endpoint;
};*/

class ServiceActuatorParams : public ServiceParams {
public:
    ServiceActuatorParams(
            const UUIDPtr& sid,
            const list<SensorInfoPtr>& sensorList,
            ActuatorQoS* qos);
    ServiceActuatorParams(
            const UUIDPtr& sid,
            const list<SensorInfoPtr>& sensorList,
            ActuatorQoS* qos,
            FTServiceParamsPtr& ftParams);

    ServiceActuatorParams(ServiceParams* params);

    virtual ~ServiceActuatorParams();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&) {
        deserializeMetadata(inputStream);
        UInt size = 0;
        inputStream.read_ulong(size);
        deserializeQuerySpecialization(inputStream);
    }

    ActuatorQoS* getQoS();
    list<SensorInfoPtr>& getSensorList();    

    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
        ULong size = m_sensorList.size();
        outputStream.write_ulong(size);
        list<SensorInfoPtr>::iterator iter = m_sensorList.begin();
        while(iter != m_sensorList.end()){
            (*iter)->serialize(outputStream);
            iter++;
        }
        m_qos->serialize(outputStream);
    }

protected:
    ActuatorQoS* m_qos;
    list<SensorInfoPtr> m_sensorList;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
        ByteInputStream inputStream(mb);
        deserializeQuerySpecialization(inputStream);
    }

    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
        ULong size = 0;
        inputStream.read_ulong(size);
        for (int i = 0; i < size; i++) {
            SensorInfo* info = new SensorInfo(inputStream);
            SensorInfoPtr sd(info);
            m_sensorList.push_back(sd);
        }
        if (m_qos != 0) {
            delete m_qos;
        }
        m_qos = new ActuatorQoS(inputStream);
    }

};

#endif	/* SERVICEActuatorPARAMS_H */

