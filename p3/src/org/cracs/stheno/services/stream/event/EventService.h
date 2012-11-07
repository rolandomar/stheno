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
 * File:   EventService.h
 * Author: Rolando Martins (rolando.martins@gmail.com)
 *
 * Created on May 8, 2010, 7:36 PM
 */

#ifndef EVENTSERVICE_H
#define	EVENTSERVICE_H

#include <stheno/service/Service.h>
#include <euryale/common/uuid/UUID.h>
#include <stheno/services/event/EventQoS.h>

class EventService: public Service {
public:       
    virtual ~EventService();

    virtual void initEvents(UUID* sensorUUID,EventQoS* qos){
    }
private:
    EventService(SthenoPtr* runtime, OverlayPtr* overlay);
};

#endif	/* EVENTSERVICE_H */

