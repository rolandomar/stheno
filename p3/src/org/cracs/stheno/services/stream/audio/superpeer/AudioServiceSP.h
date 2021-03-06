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
 * File:   AudioServiceSP.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 11, 2010, 2:41 PM
 */

#ifndef AUDIOSERVICESP_H
#define	AUDIOSERVICESP_H

#include <stheno/services/audio/AudioService.h>
class Stheno;

class AudioServiceSP: public AudioService {
public:
       
    virtual ~AudioServiceSP();

    void open();
    void close();

    static const UUID& getSID(){
        return m_sid;
    }
    static const char* getSIDStr(){
        return "b432068acc82a764860659cc45da6cbe";
    }
private:
    static UUID m_sid;
    AudioServiceSP(SthenoPtr* runtime, OverlayPtr* overlay);
};

#endif	/* AUDIOSERVICESP_H */

