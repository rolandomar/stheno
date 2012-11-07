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
 * File:   ServiceListener.h
 * Author: rmartins
 *
 * Created on August 4, 2010, 3:20 PM
 */

#ifndef SERVICELISTENER_H
#define	SERVICELISTENER_H

#include <stheno/service/ServiceAbstract.h>
class ServiceListener{
public:
    virtual void onServiceClose(const ServiceAbstract* service) = 0;
};


#endif	/* SERVICELISTENER_H */

