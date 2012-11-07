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
 * File:   SensorInfo.h
 * Author: rmartins
 *
 * Created on November 19, 2010, 2:36 PM
 */

#ifndef SENSORINFO_H
#define	SENSORINFO_H

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <euryale/Types.h>
#include <euryale/serialization/Serializable.h>
#include <ace/Refcounted_Auto_Ptr.h>

class SensorInfo: public Serializable {
public:
    SensorInfo(int type,UUIDPtr& uuid,EndpointPtr& e);
    SensorInfo(InputStream& inputStream);
    SensorInfo(const SensorInfo& orig);
    virtual ~SensorInfo();
    
    
    virtual void serialize(OutputStream& outputStream) throw (SerializationException&);    
    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    
    int getType();
    UUIDPtr& getUUID();
    EndpointPtr& getEndpoint();
protected:
    int m_type;
    UUIDPtr m_uuid;
    EndpointPtr m_endpoint;    
};
typedef ACE_Strong_Bound_Ptr<SensorInfo, ACE_Recursive_Thread_Mutex> SensorInfoPtr;

#endif	/* SENSORINFO_H */

