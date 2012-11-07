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
 * File:   GetSensorsQueryReply.h
 * Author: rmartins
 *
 * Created on October 14, 2010, 12:28 PM
 */

#ifndef GETSENSORSQUERYREPLY_H
#define	GETSENSORSQUERYREPLY_H


#include <list>
using std::list;

#include <euryale/common/uuid/UUID.h>
#include <euryale/net/endpoint/Endpoint.h>
#include <stheno/core/p2p/discovery/DiscoveryQueryReply.h>
#include <stheno/core/p2p/common/PeerInfo.h>
#include <euryale/common/io/ByteInputStream.h>

#include <stheno/core/p2p/discovery/general_provider/SensorInfo.h>

class GetSensorsQueryReply: public DiscoveryQueryReply {
public:
    
    static const UInt GETSENSORS_QUERY_REPLY = 0xF21364;
    
    GetSensorsQueryReply(list<SensorInfoPtr>& audioSensorList,list<SensorInfoPtr>& videoSensorList,
    list<SensorInfoPtr>& eventSensorList,
    list<SensorInfoPtr>& actuatorSensorList);
    
    GetSensorsQueryReply(DiscoveryQueryReply* query);
    virtual ~GetSensorsQueryReply();
   
    list<SensorInfoPtr>& getAudioSensorList();
    list<SensorInfoPtr>& getVideoSensorList();
    list<SensorInfoPtr>& getEventSensorList();
    list<SensorInfoPtr>& getActuatorSensorList();

    virtual void deserialize(InputStream& inputStream) throw (SerializationException&);
    virtual void serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&);
protected:
    //list<PeerInfoPtr> m_sensorList;
    list<SensorInfoPtr> m_audioSensorList;
    list<SensorInfoPtr> m_videoSensorList;
    list<SensorInfoPtr> m_eventSensorList;
    list<SensorInfoPtr> m_actuatorSensorList;

    void deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&);
    void deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&);
};


#endif	/* GETSENSORSQUERYREPLY_H */

