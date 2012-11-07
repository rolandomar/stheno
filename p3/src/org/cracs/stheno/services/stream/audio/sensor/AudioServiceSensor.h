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
 * File:   AudioServiceSensor.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 11, 2010, 2:41 PM
 */

#ifndef AUDIOSERVICESENSOR_H
#define	AUDIOSERVICESENSOR_H

#include <stheno/services/audio/AudioService.h>
class Stheno;

class AudioServiceSensor: public AudioService {
public:
    friend class Stheno;
    virtual ~AudioServiceSensor();

    static const UUID& getSID(){
        return m_sid;
    }
    static const char* getSIDStr(){
        return "094c57041a707dc479c86059be42ac34";
    }
private:
    static UUID m_sid;
    AudioServiceSensor(SthenoPtr* runtime, OverlayPtr* overlay);
};

#endif	/* AUDIOSERVICESENSOR_H */

