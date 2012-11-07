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
 * File:   AudioClient.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 9, 2010, 9:55 PM
 */

#include "AudioClient.h"
#include "AudioService.h"

AudioClient::AudioClient(Stheno* runtime) :
StreamClient(runtime) {
    AudioService* s = new AudioService();
    s->getSID(m_sid);
    delete s;
}

AudioClient::~AudioClient() {
}

const UUIDPtr& AudioClient::getSID() {
    return m_sid;
}

int AudioClient::open(const UUIDPtr& sid, const UUIDPtr& iid,ClientParamsPtr& cp) {
    return StreamClient::open(sid, iid,cp,0);
}