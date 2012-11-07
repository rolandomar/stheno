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
 * File:   AudioServiceInterface.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 10, 2010, 11:28 AM
 */

#ifndef AUDIOSERVICEINTERFACE_H
#define	AUDIOSERVICEINTERFACE_H

#include <euryale/common/uuid/UUID.h>
#include <stheno/services/stream/audio/AudioQoS.h>
#include <stheno/core/p2p/net/packet/SthenoPacket.h>

class AudioListener{
public:
    virtual void onAudioPacket(SthenoPacket* packet) = 0;
};

class AudioServiceInterface {
public:
    //virtual void initAudioStream(UUID* sensorUUID, AudioQoS* qos) = 0;
    //virtual void cancelAudioStream(UUID* sensorUUID) = 0;
    virtual void changeAudioStreamQoS(UUID* sensorUUID, AudioQoS* qos) = 0;
    virtual void onAudioPacket(SthenoPacket* packet) = 0;
};

#endif	/* AUDIOSERVICEINTERFACE_H */

