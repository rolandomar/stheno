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
 * File:   SensorInfoUpdate.h
 * Author: rmartins
 *
 * Created on November 17, 2010, 4:45 PM
 */

#ifndef SENSORINFOUPDATE_H
#define	SENSORINFOUPDATE_H

#include <stheno/core/p2p/p3/mesh/InfoUpdate.h>
#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>

class SensorInfoUpdate: public InfoUpdate {
public:
    static const int SENSOR_UPDATE_TYPE = 0x1324821;
    static const int SENSOR_UPDATE_AUDIO_OP = 0x01;
    static const int SENSOR_UPDATE_VIDEO_OP = 0x02;
    static const int SENSOR_UPDATE_EVENT_OP = 0x03;
    
    static const int SENSOR_UPDATE_ACTUATOR_OP = 0x04;
    
    SensorInfoUpdate(int op,UUIDPtr& uuid,EndpointPtr& e);    
    SensorInfoUpdate(InfoUpdate* info,InputStream& inputStream);
    SensorInfoUpdate(InputStream& inputStream);
    virtual ~SensorInfoUpdate();
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    
    UUIDPtr& getUUID();    
    EndpointPtr& getEndpoint();
    
protected:
    UUIDPtr m_uuid;
    EndpointPtr m_endpoint;
        
    virtual void serializeSpecializationBody(OutputStream& outputStream) throw (SerializationException&);
    virtual void deserializeSpecializationBody(InputStream& inputStream) throw (SerializationException&);
};
#endif	/* SENSORINFOUPDATE_H */

