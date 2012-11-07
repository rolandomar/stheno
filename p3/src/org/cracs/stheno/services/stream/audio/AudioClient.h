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
 * File:   AudioClient.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 9, 2010, 9:55 PM
 */

#ifndef AUDIOCLIENT_H
#define	AUDIOCLIENT_H
#include <stheno/services/stream/audio/AudioServiceInterface.h>
class Stheno;
//#include <stheno/service/ServiceClient.h>
#include <stheno/services/stream/StreamClient.h>

class AudioClient : public StreamClient, public AudioServiceInterface {
public:
    friend class Stheno;
    friend class AudioService;
    virtual ~AudioClient();

    int open(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp);
    
    void setAudiolistener(AudioListener* listener){
        m_listener = listener;
    }

    //sensorUUID reflects an instance of a AudioServiceSensor

    /*void initAudioStream(UUID* sensorUUID,AudioQoS* qos){}
      
    void cancelAudioStream(UUID* sensorUUID) {}*/

    void changeAudioStreamQoS(UUID* sensorUUID, AudioQoS* qos) {
    }

    void onAudioPacket(SthenoPacket* packet) {
        m_listener->onAudioPacket(packet);
    }

    const UUIDPtr& getSID();


    virtual void close() {
    }


private:    
    AudioListener* m_listener;
    UUIDPtr m_sid;

    AudioClient(Stheno* runtime);


};

#endif	/* AUDIOCLIENT_H */

