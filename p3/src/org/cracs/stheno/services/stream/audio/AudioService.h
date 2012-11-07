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
 * File:   AudioService.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 8, 2010, 7:35 PM
 */

#ifndef AUDIOSERVICE_H
#define	AUDIOSERVICE_H

#include <stheno/services/stream/StreamService.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/services/stream/audio/AudioQoS.h>

/**
 * Implementation of the Audio service on top of P2P layer
 * This service will be automatic deployed on the nodes,
 * no manual deployment is going to be allowed
 */
class AudioService : public StreamService {
public:
    friend class AudioServiceSensor;
    friend class AudioServiceSP;
    AudioService();    
    AudioService(Stheno* runtime);
    AudioService(Stheno* runtime,UUIDPtr& iid);    
    virtual ~AudioService();

    virtual QoSResources* calculateQoSResources(ServiceParamsPtr& params);
    virtual ServiceAbstract* duplicate() throw (ServiceException&);
    virtual const char* getName();
    //virtual list<EndpointPtr>& getEndpoints() throw (ServiceException&);
    virtual const char* id();

    virtual void open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&);

    void initAudioStream(UUID* sensorUUID, AudioQoS* qos) {

    }

    void cancelAudioStream(UUID* sensorUUID) {
    }

    void changeAudioStreamQoS(UUID* sensorUUID, AudioQoS* qos) {
    }
    
    //virtual SAPInfoPtr& getSAP(){ return m_sap;}
    
    virtual ServiceClient* getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    
protected:
    //SAPInfoPtr m_sap;
};

#endif	/* AUDIOSERVICE_H */

