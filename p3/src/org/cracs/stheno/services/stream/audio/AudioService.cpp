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
 * File:   AudioService.cpp
 * Author: Rolando Martins (rolando.martins@gmail.com)
 * 
 * Created on May 8, 2010, 7:35 PM
 */

#include "AudioService.h"
#include "AudioClient.h"
#include <stheno/Stheno.h>


AudioService::AudioService():StreamService(0,id()) {
}

AudioService::AudioService(Stheno* runtime):StreamService(runtime,id()) {
}

AudioService::AudioService(Stheno* runtime,UUIDPtr& iid):StreamService(runtime,id(),iid) {
}


AudioService::~AudioService() {
}

QoSResources* AudioService::calculateQoSResources(ServiceParamsPtr& params){
    return 0;
}

ServiceAbstract* AudioService::duplicate() throw (ServiceException&){
    return new AudioService(m_runtime);
}

const char* AudioService::getName(){
    return "AudioService";
}

const char* AudioService::id(){
    return "2A55C3F005459F672057661FBE9C700A";
}

void AudioService::open_i(ServiceParamsPtr& params, int fttype) throw (ServiceException&){    
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: AudioService::open_i()\n")));
    if(params == 0){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ERROR: AudioService::open_i() - failed 1\n")));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }        
    UUIDPtr sid;
    getSID(sid);
    if(!params->getSID()->isEqual(sid)){
        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)ERROR: AudioService::open_i() - failed 2\n")));
        throw ServiceException(ServiceException::INVALID_ARGUMENT);
    }
    StreamService::open_i(params,fttype);    
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t|%T)INFO: AudioService::open_i() - OK\n")));
}


ServiceClient* AudioService::getClient(const UUIDPtr& sid,const UUIDPtr& iid,ClientParamsPtr& cp){
    AudioClient* c = new AudioClient(this->getRuntime()->duplicate());
    c->open(sid,iid,cp);
    return c;
}