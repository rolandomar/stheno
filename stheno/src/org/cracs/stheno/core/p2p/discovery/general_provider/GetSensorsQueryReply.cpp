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
 * File:   GetSensorsQueryReply.cpp
 * Author: rmartins
 * 
 * Created on October 14, 2010, 12:28 PM
 */

#include "GetSensorsQueryReply.h"
#include "GeneralProvider.h"

GetSensorsQueryReply::GetSensorsQueryReply(list<SensorInfoPtr>& audioSensorList, list<SensorInfoPtr>& videoSensorList, 
        list<SensorInfoPtr>& eventSensorList,list<SensorInfoPtr>& actuatorSensorList) :
DiscoveryQueryReply(GENERALPROVIDER_TYPE, GETSENSORS_QUERY_REPLY),
m_audioSensorList(audioSensorList), m_videoSensorList(videoSensorList), m_eventSensorList(eventSensorList),m_actuatorSensorList(actuatorSensorList) {

}

GetSensorsQueryReply::~GetSensorsQueryReply() {

}

GetSensorsQueryReply::GetSensorsQueryReply(DiscoveryQueryReply* query) : DiscoveryQueryReply(query) {
    if (m_ex == 0) {
        this->deserializeQuerySpecialization(this->deattachQuerySpecializationBlob());
    }
}

list<SensorInfoPtr>& GetSensorsQueryReply::getAudioSensorList() {
    return m_audioSensorList;
}

list<SensorInfoPtr>& GetSensorsQueryReply::getVideoSensorList() {
    return m_videoSensorList;
}

list<SensorInfoPtr>& GetSensorsQueryReply::getEventSensorList() {
    return m_eventSensorList;
}

list<SensorInfoPtr>& GetSensorsQueryReply::getActuatorSensorList() {
    return m_actuatorSensorList;
}

void GetSensorsQueryReply::deserialize(InputStream& inputStream) throw (SerializationException&) {
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

void GetSensorsQueryReply::serializeQuerySpecialization(OutputStream& outputStream) throw (SerializationException&) {
    // m_info->serialize(outputStream);

    int size = m_audioSensorList.size();
    outputStream.write_long(size);

    list<SensorInfoPtr>::iterator iter = m_audioSensorList.begin();
    while (iter != m_audioSensorList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }

    size = m_videoSensorList.size();

    outputStream.write_long(size);
    iter = m_videoSensorList.begin();
    while (iter != m_videoSensorList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }

    size = m_eventSensorList.size();

    outputStream.write_long(size);
    iter = m_eventSensorList.begin();
    while (iter != m_eventSensorList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
    
    size = m_actuatorSensorList.size();

    outputStream.write_long(size);
    iter = m_actuatorSensorList.begin();
    while (iter != m_actuatorSensorList.end()) {
        (*iter)->serialize(outputStream);
        iter++;
    }
}

void GetSensorsQueryReply::deserializeQuerySpecialization(ACE_Message_Block* mb) throw (SerializationException&) {
    ByteInputStream inputStream(mb);
    deserializeQuerySpecialization(inputStream);
}

void GetSensorsQueryReply::deserializeQuerySpecialization(InputStream& inputStream) throw (SerializationException&) {
    int size = 0;
    m_audioSensorList.clear();
    inputStream.read_long(size);
    for (int i = 0; i < size; i++) {
        SensorInfo* info = new SensorInfo(inputStream);
        SensorInfoPtr infoPtr(info);
        m_audioSensorList.push_back(infoPtr);
    }

    m_videoSensorList.clear();
    inputStream.read_long(size);
    for (int i = 0; i < size; i++) {
        SensorInfo* info = new SensorInfo(inputStream);
        SensorInfoPtr infoPtr(info);
        m_videoSensorList.push_back(infoPtr);
    }

    m_eventSensorList.clear();

    inputStream.read_long(size);

    for (int i = 0; i < size; i++) {
        SensorInfo* info = new SensorInfo(inputStream);
        SensorInfoPtr infoPtr(info);
        m_eventSensorList.push_back(infoPtr);

    }
    
    m_actuatorSensorList.clear();

    inputStream.read_long(size);

    for (int i = 0; i < size; i++) {
        SensorInfo* info = new SensorInfo(inputStream);
        SensorInfoPtr infoPtr(info);
        m_actuatorSensorList.push_back(infoPtr);

    }
}